/**
 * @file SnmpParamsController.cpp
 */

// Own includes
#include "controller/SnmpParamsController.h"
#include "gui/BinaryButtonView.h"
#include "gui/EditTextView.h"
#include "gui/ButtonView.h"
#include "gui/UpdateView.h"
#include "Application.h"
#include "snmp/Snmpv2Pdu.h"
#include "snmp/Snmpv3Pdu.h"
#include "asn1/BerInteger.h"
#include "Config.h"
#include "Utils.h"

// Defines
#define LOADING_SPEED   0.05f
#define STACKSIZE       (16 << 10)

namespace NetMan {

static void editSnmpVersion(void *args) {
    BinaryButtonParams *params = (BinaryButtonParams*)args;
    auto controller = std::static_pointer_cast<SnmpParamsController>(params->controller);
    if(controller->getThreadParams()->working) return;
    if(!params->init) return;
    controller->setUsmEnabled(params->selected);
}

static void editCommunity(void *args) {
    EditTextParams *params = (EditTextParams*)args;
    auto controller = std::static_pointer_cast<SnmpParamsController>(params->controller);
    if(controller->getThreadParams()->working) return;
    if(!params->init) return;
    controller->setCommunity(params->text);
}

static void editUsername(void *args) {
    EditTextParams *params = (EditTextParams*)args;
    auto controller = std::static_pointer_cast<SnmpParamsController>(params->controller);
    if(controller->getThreadParams()->working) return;
    if(!params->init) return;
    controller->setUsername(params->text);
}

static void sendRequest(void *args) {

    SnmpThreadParams *params = (SnmpThreadParams*)args;
    auto session = params->session;
    params->error = false;
    params->success = false;
    params->working = true;
   
    for(u32 i = 0; i < 60; i++) gspWaitForVBlank();

    // Prepare OIDs
    auto& pduFields = Application::getInstance().getPduFields();
    for(u32 i = 0; i < pduFields.size(); i++) {
        if(session->pduType == SNMPV1_GETREQUEST || session->pduType == SNMPV1_SETREQUEST ||
           (session->pduType == SNMPV2_GETBULKREQUEST && i < session->nonRepeaters)) {
            pduFields[i].oid->addElement(0);    // Add .0 to scalar objects
        }
    }

    // Send the SNMP PDU
    try {
        Utils::sendSnmpPdu(params);
    } catch (const std::runtime_error &e) {
        params->working = false;
        params->error = true;
        params->errorText = e.what();
        return;
    }

    params->working = false;
    params->success = true;
}

static void sendSnmp(void *args) {

    ButtonParams *params = (ButtonParams*)args;
    auto controller = std::static_pointer_cast<SnmpParamsController>(params->controller);
    SnmpThreadParams *threadParams = controller->getThreadParams();
    if(threadParams->working) return;
   
    if(threadParams->session->isTable) {
        auto contextData = std::make_shared<SnmpThreadParams>();
        contextData->session = threadParams->session;
        contextData->community = threadParams->community;
        contextData->username = threadParams->username;
        contextData->usmEnabled = threadParams->usmEnabled;
        Application::getInstance().requestLayoutChange("snmptable", contextData);
    } else {
        s32 prio = 0;
        svcGetThreadPriority(&prio, CUR_THREAD_HANDLE);
        Thread thread = threadCreate(sendRequest, threadParams, STACKSIZE, prio-1, -2, true);
        if(thread == NULL) {
            Application::getInstance().messageBox("Can't create thread for sending SNMP request");
        } else {
            controller->getLoadingBox()->setShow(true);
            controller->getLoadingIcon()->setShow(true);
        }
    }
}

static void updateRequest(void *args) {
    UpdateParams *params = (UpdateParams*)args;
    auto controller = std::static_pointer_cast<SnmpParamsController>(params->controller);
    auto threadParams = controller->getThreadParams();

    if(!threadParams->working) {
        if(threadParams->error) {
            Application::getInstance().messageBox("An error has ocurred:\n" + threadParams->errorText);
            threadParams->error = false;
            controller->getLoadingBox()->setShow(false);
            controller->getLoadingIcon()->setShow(false);
        } else if(threadParams->success) {
            Application::getInstance().requestLayoutChange("snmpresult");
            controller->getLoadingBox()->setShow(false);
            controller->getLoadingIcon()->setShow(false);
        }
    } else {
        controller->getLoadingIcon()->addRotation(LOADING_SPEED);
    }
}

static void goBack(void *args) {
    Application::getInstance().requestLayoutChange("snmp");
}

SnmpParamsController::SnmpParamsController() {

    this->cbMap = std::unordered_map<std::string, void(*)(void*)> {
        {"editSnmpVersion", editSnmpVersion},
        {"editCommunity", editCommunity},
        {"editUsername", editUsername},
        {"sendSnmp", sendSnmp},
        {"goBack", goBack},
        {"updateRequest", updateRequest},
    };

    threadParams.usmEnabled = false;
    threadParams.community = "";
    threadParams.username = "";

    auto contextData = std::static_pointer_cast<SnmpSessionParams>(Application::getInstance().getContextData());
    if(contextData == nullptr) {
        throw std::runtime_error("No context specified");
    }
    this->threadParams.session = contextData;
    this->threadParams.working = false;
    this->threadParams.error = false;
    this->threadParams.success = false;
}

void SnmpParamsController::initialize(std::vector<std::shared_ptr<GuiView>> &views) {

    loadingBox = std::static_pointer_cast<ImageView>(views[12]);
    loadingIcon = std::make_shared<ImageView>("loading", loadingBox->getX(), loadingBox->getY(), 1.0f, 1.0f);

    loadingBox->setShow(false);
    loadingIcon->setShow(false);

    views.push_back(loadingIcon);
}

SnmpParamsController::~SnmpParamsController() { }

}
