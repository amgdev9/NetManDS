/**
 * @file AgentViewController.cpp
 */

// Own includes
#include "controller/AgentViewController.h"
#include "Application.h"
#include "gui/ListView.h"
#include "gui/ImageView.h"
#include "gui/TextView.h"
#include "gui/ButtonView.h"
#include "Utils.h"

// Defines
#define ICON_SIZE           81.0f
#define TEXT_OFFX           10.0f
#define TEXT_SCALE          0.8f
#define MAX_STR_LENGTH      21
#define AGENTS_PATH         "lastScan.json"

namespace NetMan {

/**
 * @brief Go to the Agent Discovery screen or the SNMP screen
 */
static void goBack(void *args) {
    ButtonParams *params = (ButtonParams*)args;
    auto controller = std::static_pointer_cast<AgentViewController>(params->controller);
    if(controller->getComesFromAgentDiscovery()) {
        Application::getInstance().requestLayoutChange("agentdiscovery");
    } else {
        Application::getInstance().requestLayoutChange("snmp");
    }
}

/**
 * @brief Fill the agents list
 */
static void fillAgents(void *args) {

    ListViewFillParams *params = (ListViewFillParams*)args;
    auto controller = std::static_pointer_cast<AgentViewController>(params->controller);
    params->layouts.clear();

    auto list = controller->getJson();
    if(list.get() == NULL) {
        Application::getInstance().messageBox("No agents found. You should do an agent scan first");
        Application::getInstance().requestLayoutChange("agentdiscovery");
        return;
    }
    u32 listSize = json_array_size(list.get());

    if(params->endElement >= listSize) {
        params->remaining = false;
    }

    for(u32 i = params->startElement; i < params->endElement; i++) {
        if(i < listSize) {
            float y = params->startY + (i % params->maxElements) * params->elementHeight;
            json_t *obj = json_array_get(list.get(), i);
            json_t *ip = json_object_get(obj, "ip");
            const char *ipString = json_string_value(ip);

            std::shared_ptr<GuiLayout> layout = std::make_shared<GuiLayout>();
            std::shared_ptr<ImageView> bg = std::make_shared<ImageView>("menuButton", params->startX + params->elementWidth / 2, y + params->elementHeight / 2, params->elementWidth / ICON_SIZE, params->elementHeight / ICON_SIZE);
            layout->addView(bg);
            
            if(ipString) {
                std::shared_ptr<TextView> tv = std::make_shared<TextView>(ipString, params->startX + TEXT_OFFX, y, TEXT_SCALE, C2D_Color32(0, 0, 0, 0xFF));
                layout->addView(tv);
            }

            params->layouts.push_back(layout);
        } else {
            i = params->endElement;
            params->remaining = false;
        }
    }
}

/**
 * @brief Called when an agent is clicked
 */
static void clickAgent(void *args) {
    ListViewClickParams *params = (ListViewClickParams*)args;
    auto controller = std::static_pointer_cast<AgentViewController>(params->controller);
    auto list = controller->getJson();
    json_t *obj = json_array_get(list.get(), params->element);

    if(controller->getComesFromAgentDiscovery()) {
        // Show agent data
        json_t *data = json_object_get(obj, "data");
        const char *dataString = json_string_value(data);

        if(dataString) {
            Application::getInstance().messageBox(dataString);
        }
    } else {
        // Store agent IP
        auto sessionParams = controller->getSnmpSessionParams();
        json_t *ip = json_object_get(obj, "ip");
        sessionParams->agentIP = inet_addr(json_string_value(ip));
        Application::getInstance().requestLayoutChange("snmpparams", sessionParams);
    }
}

/**
 * @brief Constructor for an AgentViewController
 */
AgentViewController::AgentViewController() {
    this->cbMap = std::unordered_map<std::string, void(*)(void*)> {
        {"goBack", goBack},
        {"fillAgents", fillAgents},
        {"clickAgent", clickAgent},
    };

    root = Utils::loadJsonList(AGENTS_PATH);

    if(Application::getInstance().getContextData() != nullptr) {
        this->comesFromAgentDiscovery = false;
        this->snmpSessionParams = std::static_pointer_cast<SnmpSessionParams>(Application::getInstance().getContextData());
    } else {
        this->comesFromAgentDiscovery = true;
    }
}

/**
 * @brief Destructor for an AgentViewController
 */
AgentViewController::~AgentViewController() { }

}
