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

static std::shared_ptr<BerField> prepareSetField(u32 i) {

    std::shared_ptr<BerField> currentField = nullptr;
    auto &pduFields = Application::getInstance().getPduFields();

    switch(pduFields[i].type) {
        case 'i':       // Integer
        {
            s32 val = strtol(pduFields[i].value.c_str(), NULL, 10);
            currentField = std::make_shared<BerInteger>(&val, sizeof(s32), true);
        } break;
        case 's':       // Octet string
            currentField = std::make_shared<BerOctetString>(pduFields[i].value);
            break;
        case 'n':       // Null
            currentField = std::make_shared<BerNull>();
            break;
        case 'O':       // Object Identifier
            currentField = std::make_shared<BerOid>(pduFields[i].value);
            break;
        case 'a':       // Network Address
        {
            char ipOctetString[5];
            in_addr_t ip = inet_addr(pduFields[i].value.c_str());
            if(ip == INADDR_NONE) {
                throw std::runtime_error("Field " + pduFields[i].oidText + " is not a valid IP address");
            }
            memcpy(ipOctetString, &ip, 4);
            ipOctetString[4] = '\0';
            currentField = std::make_shared<BerOctetString>(ipOctetString, SNMPV1_TAGCLASS_NETWORKADDRESS, SNMPV1_TAG_NETWORKADDRESS);
        } break;
        case 'c':       // Counter
        {
            u32 val = strtoul(pduFields[i].value.c_str(), NULL, 10);
            currentField = std::make_shared<BerInteger>(&val, sizeof(u32), false, SNMPV1_TAGCLASS_COUNTER, SNMPV1_TAG_COUNTER);
        } break;
        case 'C':       // Counter64
        {
            u64 val = strtoul(pduFields[i].value.c_str(), NULL, 10);
            currentField = std::make_shared<BerInteger>(&val, sizeof(u64), false, SNMPV2_TAGCLASS_COUNTER64, SNMPV2_TAG_COUNTER64);
        } break;
        case 'g':       // Gauge
        {
            u32 val = strtoul(pduFields[i].value.c_str(), NULL, 10);
            currentField = std::make_shared<BerInteger>(&val, sizeof(u32), false, SNMPV1_TAGCLASS_GAUGE, SNMPV1_TAG_GAUGE);
        } break;
        case 't':       // TimeTicks
        {
            u32 val = strtoul(pduFields[i].value.c_str(), NULL, 10);
            currentField = std::make_shared<BerInteger>(&val, sizeof(u32), false, SNMPV1_TAGCLASS_TIMETICKS, SNMPV1_TAG_TIMETICKS);
        } break;
        case 'o':       // Opaque
            currentField = std::make_shared<BerOctetString>(pduFields[i].value, SNMPV1_TAGCLASS_OPAQUE, SNMPV1_TAG_OPAQUE);
            break;
        default:
            throw std::runtime_error("Field " + pduFields[i].oidText + " has no valid ASN.1 syntax");
    }

    return currentField;
}

static void sendRequest(void *args) {

    SnmpThreadParams *params = (SnmpThreadParams*)args;
    auto session = params->session;
    params->error = false;
    params->success = false;
    params->working = true;
   
    for(u32 i = 0; i < 60; i++) gspWaitForVBlank();

    auto& configStore = Config::getInstance();
    auto& config = configStore.getData();
    std::shared_ptr<UdpSocket> sock = std::make_shared<UdpSocket>(config.udpTimeout);

    if(session->isTable) {
        // TODO Handle table
    } else {
        // Prepare OIDs
        auto& pduFields = Application::getInstance().getPduFields();
        for(u32 i = 0; i < pduFields.size(); i++) {
            if(session->pduType == SNMPV1_GETREQUEST || session->pduType == SNMPV1_SETREQUEST ||
               (session->pduType == SNMPV2_GETBULKREQUEST && i < session->nonRepeaters)) {
                pduFields[i].oid->addElement(0);    // Add .0 to scalar objects
            }
        }

        try {
            if(params->usmEnabled) {
                std::shared_ptr<Snmpv3Pdu> pdu = std::make_shared<Snmpv3Pdu>(configStore.getEngineID(), configStore.getContextName(), params->username);
                std::shared_ptr<BerNull> nullval = std::make_shared<BerNull>();
                std::shared_ptr<BerOid> testOid = std::make_shared<BerOid>("1.3.6.1.2.1.1.7.0");
                pdu->addVarBind(testOid, nullval);
                pdu->sendRequest(SNMPV2_GETREQUEST, sock, session->agentIP, config.snmpPort);
                try {
                    pdu->recvResponse(sock, session->agentIP, config.snmpPort);
                } catch (const std::runtime_error &e) { }
                pdu->clear();

                if(session->pduType == SNMPV1_SETREQUEST) {
                    for(u32 i = 0; i < pduFields.size(); i++) {
                        pdu->addVarBind(pduFields[i].oid, prepareSetField(i));
                    }
                } else {
                    for(u32 i = 0; i < pduFields.size(); i++) {
                        pdu->addVarBind(pduFields[i].oid, nullval);
                    }
                }

                if(session->pduType == SNMPV2_GETBULKREQUEST) {
                    pdu->sendBulkRequest(session->nonRepeaters, session->maxRepetitions, sock, session->agentIP, config.snmpPort);
                } else {
                    pdu->sendRequest(session->pduType, sock, session->agentIP, config.snmpPort);
                }
                for(u32 i = 0; i < 60; i++) gspWaitForVBlank();
                pdu->recvResponse(sock, session->agentIP, config.snmpPort);

                if(session->pduType == SNMPV2_GETBULKREQUEST) {
                    for(u32 i = 0; i < pdu->getNVarBinds(); i++) {
                        if(i < session->nonRepeaters) {
                            pduFields[i].value = pdu->getVarBind(i)->print();
                        } else if(i == session->nonRepeaters) {
                            pduFields[i].value = std::to_string(pdu->getNVarBinds() - session->nonRepeaters) + " fields";
                        } else {
                            PduField field;
                            field.oidText = pdu->getVarBindOid(session->nonRepeaters)->print();
                            field.value = pdu->getVarBind(i)->print();
                            pduFields.push_back(field);
                        }
                    }
                } else {
                    for(u32 i = 0; i < pduFields.size(); i++) {
                        pduFields[i].value = pdu->getVarBind(i)->print();
                    }
                }
            } else {
                if(session->pduType == SNMPV2_GETBULKREQUEST) {
                    std::unique_ptr<Snmpv2Pdu> pdu = std::unique_ptr<Snmpv2Pdu>(new Snmpv2Pdu(params->community));
                    std::shared_ptr<BerNull> nullval = std::make_shared<BerNull>();

                    for(u32 i = 0; i < pduFields.size(); i++) {
                        pdu->addVarBind(pduFields[i].oid, nullval);
                    }

                    pdu->sendBulkRequest(session->nonRepeaters, session->maxRepetitions, sock, session->agentIP, config.snmpPort);
                    pdu->recvResponse(sock, session->agentIP, config.snmpPort);

                    for(u32 i = 0; i < pdu->getNVarBinds(); i++) {
                        if(i < session->nonRepeaters) {
                            pduFields[i].value = pdu->getVarBind(i)->print();
                        } else if(i == session->nonRepeaters) {
                            pduFields[i].value = std::to_string(pdu->getNVarBinds() - session->nonRepeaters) + " fields";
                        } else {
                            PduField field;
                            field.oidText = pdu->getVarBindOid(session->nonRepeaters)->print();
                            field.value = pdu->getVarBind(i)->print();
                            pduFields.push_back(field);
                        }
                    }
                } else {
                    std::unique_ptr<Snmpv1Pdu> pdu = std::unique_ptr<Snmpv1Pdu>(new Snmpv1Pdu(params->community));
                    if(session->pduType == SNMPV1_SETREQUEST) {
                        for(u32 i = 0; i < pduFields.size(); i++) {
                            pdu->addVarBind(pduFields[i].oid, prepareSetField(i));
                        }
                    } else {
                        std::shared_ptr<BerNull> nullval = std::make_shared<BerNull>();
                        for(u32 i = 0; i < pduFields.size(); i++) {
                            pdu->addVarBind(pduFields[i].oid, nullval);
                        }
                    }

                    pdu->sendRequest(session->pduType, sock, session->agentIP, config.snmpPort);
                    pdu->recvResponse(sock, session->agentIP, config.snmpPort);

                    for(u32 i = 0; i < pduFields.size(); i++) {
                        pduFields[i].value = pdu->getVarBind(i)->print();
                    }
                }
            }
        } catch (const std::runtime_error &e) {
            params->working = false;
            params->error = true;
            params->errorText = e.what();
            return;
        }
    }

    params->working = false;
    params->success = true;
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
    } else {
        controller->getLoadingBox()->setShow(true);
        controller->getLoadingIcon()->setShow(true);
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
