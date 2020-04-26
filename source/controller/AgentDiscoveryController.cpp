/**
 * @file AgentDiscoveryController.cpp
 */

// Own includes
#include "controller/AgentDiscoveryController.h"
#include "gui/ButtonView.h"
#include "gui/UpdateView.h"
#include "Application.h"
#include "snmp/SnmpAgentScanner.h"
#include "Utils.h"
#include "Config.h"

// Defines
#define PROGRESSTEXT_X      270.0f
#define PROGRESSTEXT_Y      202.0f
#define PROGRESSTEXT_SCALE  0.6f
#define STACKSIZE           (16 << 10)
#define SCAN_RESULT_PATH    "lastScan.json"

namespace NetMan {

static void goSnmp(void *args) {
    ButtonParams *params = (ButtonParams*)args;
    auto controller = std::static_pointer_cast<AgentDiscoveryController>(params->controller);
    if(controller->getParams().scanning) return;
    Application::getInstance().requestLayoutChange("snmp");
}

static void editRange(void *args) {
    EditTextParams *params = (EditTextParams*)args;
    auto controller = std::static_pointer_cast<AgentDiscoveryController>(params->controller);
    if(controller->getParams().scanning || !params->init) return;

    char *colon = strchr(params->text, ':');
    if(colon == NULL) {
        params->init = false;
    } else {
        colon[0] = 0;
        in_addr_t ipAddr = inet_addr(params->text);
        if(ipAddr == INADDR_NONE) {
            params->init = false;
        } else {
            int nhosts = strtol(&colon[1], NULL, 10);
            if(nhosts > 0) {
                colon[0] = ':';
                controller->getParams().baseIP = ipAddr;
                controller->getParams().nhosts = nhosts;
            } else {
                params->init = false;
            }
        }
    }
}

static void editPort(void *args) {
    EditTextParams *params = (EditTextParams*)args;
    auto controller = std::static_pointer_cast<AgentDiscoveryController>(params->controller);
    if(controller->getParams().scanning) return;
    u16 port = Utils::handleFormPort(params, Config::getInstance().getData().snmpPort);
    if(port > 0) {
        controller->getParams().port = port;
    }
}

static void editVersion(void *args) {
    EditTextParams *params = (EditTextParams*)args;
    auto controller = std::static_pointer_cast<AgentDiscoveryController>(params->controller);
    if(controller->getParams().scanning) return;
    u32 *version = &controller->getParams().version;
    Utils::handleFormInteger(params, version, 2);
}

static void editMaxRequests(void *args) {
    EditTextParams *params = (EditTextParams*)args;
    auto controller = std::static_pointer_cast<AgentDiscoveryController>(params->controller);
    if(controller->getParams().scanning) return;
    Utils::handleFormInteger(params, &controller->getParams().maxRequests, 30);
}

static void editTimeout(void *args) {
    EditTextParams *params = (EditTextParams*)args;
    auto controller = std::static_pointer_cast<AgentDiscoveryController>(params->controller);
    if(controller->getParams().scanning) return;
    Utils::handleFormInteger(params, &controller->getParams().timeout, 100);
}

static void doScan(void *args) {
    AgentDiscoveryParams *params = (AgentDiscoveryParams*)args;
    params->prevProgress = 0xFF;
    params->progress = 0;
    params->scanning = true;
    params->error = false;

    try {
        std::shared_ptr<SnmpAgentScanner> agentScanner = std::make_shared<SnmpAgentScanner>();
        agentScanner->scanAgents(params->baseIP, params->nhosts, params->port, params->version - 1, params->maxRequests, params->timeout, &params->progress);
        agentScanner->dumpJson(SCAN_RESULT_PATH);
    } catch (const std::runtime_error &e) {
        params->error = true;
    }
    
    params->scanning = false;
}

static void scan(void *args) {
    ButtonParams *params = (ButtonParams*)args;
    auto controller = std::static_pointer_cast<AgentDiscoveryController>(params->controller);
    if(controller->getParams().scanning) return;
    auto &data = controller->getParams();
    if(data.baseIP == 0 || data.nhosts == 0 || data.maxRequests == 0 || data.timeout == 0) {
        Application::getInstance().messageBox("The form was not filled properly");
    } else {
        s32 prio = 0;
	    svcGetThreadPriority(&prio, CUR_THREAD_HANDLE);
		Thread thread = threadCreate(doScan, &controller->getParams(), STACKSIZE, prio-1, -2, true);
        if(thread == NULL) {
            Application::getInstance().messageBox("Can't create scanner thread");
        }
    }
}

static void onUpdateProgress(void *args) {
    
    UpdateParams *params = (UpdateParams*)args;
    auto controller = std::static_pointer_cast<AgentDiscoveryController>(params->controller);
    auto& scanParams = controller->getParams();

    if(!scanParams.scanning) {
        if(scanParams.progress == 100) {
            Application::getInstance().requestLayoutChange("agentview");
        } else if(scanParams.error) {
            Application::getInstance().messageBox("An error has ocurred in the scan. Aborting");
            scanParams.error = false;
        } else {
            return;
        }
    }

    if(scanParams.prevProgress != scanParams.progress) {
        scanParams.prevProgress = scanParams.progress;
        controller->getProgressText()->setText(std::to_string(controller->getParams().progress) + "%");
    }
}

void AgentDiscoveryController::initialize(std::vector<std::shared_ptr<GuiView>> &views) {
    progressText = std::make_shared<TextView>("", PROGRESSTEXT_X, PROGRESSTEXT_Y, PROGRESSTEXT_SCALE, 0xFF000000);
    views.push_back(progressText);
}

AgentDiscoveryController::AgentDiscoveryController() {
    this->cbMap = std::unordered_map<std::string, void(*)(void*)> {
        {"goSnmp", goSnmp},
        {"editRange", editRange},
        {"editPort", editPort},
        {"editVersion", editVersion},
        {"editMaxRequests", editMaxRequests},
        {"editTimeout", editTimeout},
        {"scan", scan},
        {"onUpdateProgress", onUpdateProgress},
    };

    memset(&params, 0, sizeof(AgentDiscoveryParams));
    params.version = 1;
    params.port = Config::getInstance().getData().snmpPort;
}

AgentDiscoveryController::~AgentDiscoveryController() { }

}
