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

    if(session->isTable) {
        // TODO Handle table
    } else {
        if(params->usmEnabled) {
            // TODO SNMPv3
        } else {
            if(session->pduType == SNMPV2_GETBULKREQUEST) {
                // TODO SNMPv2
            } else {
                // TODO SNMPv1
                std::unique_ptr<Snmpv1Pdu> pdu = std::make_unique<Snmpv1Pdu>(params->community);
            }
        }
    }

    params->working = false;
    params->success = false;
    params->error = true;
}

static void sendSnmp(void *args) {

    ButtonParams *params = (ButtonParams*)args;
    auto controller = std::static_pointer_cast<SnmpParamsController>(params->controller);
    SnmpThreadParams *threadParams = controller->getThreadParams();
    if(threadParams->working) return;
   
    s32 prio = 0;
	svcGetThreadPriority(&prio, CUR_THREAD_HANDLE);
	Thread thread = threadCreate(sendRequest, threadParams, STACKSIZE, prio-1, -2, true);
    if(thread == NULL) {
        Application::getInstance().messageBox("Can't create thread for sending SNMP request");
    }

    controller->getLoadingBox()->setShow(true);
    controller->getLoadingIcon()->setShow(true);
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
            // TODO Go to SNMP PDU results
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

    // TODO Remove test
    std::shared_ptr<SnmpSessionParams> sessionParams = std::make_shared<SnmpSessionParams>();
    sessionParams->agentIP = inet_addr("192.168.100.7");
    sessionParams->isTable = false;
    sessionParams->pduType = SNMPV1_GETREQUEST;
    threadParams.community = "public";
    threadParams.usmEnabled = false;
    threadParams.session = sessionParams;
    PduField field;
    field.oid = std::make_shared<BerOid>("1.3.6.1.2.1.1.1");
    Application::getInstance().getPduFields().push_back(field);

    /*auto contextData = std::static_pointer_cast<SnmpSessionParams>(Application::getInstance().getContextData());
    if(contextData == nullptr) {
        throw std::runtime_error("No context specified");
    }
    this->threadParams.session = contextData;*/
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
