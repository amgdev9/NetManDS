/**
 * @file AgentDiscoveryController.h
 */

#ifndef _AGENTDISCOVERY_CONTROLLER_H_
#define _AGENTDISCOVERY_CONTROLLER_H_

// Own includes
#include "GuiController.h"
#include "gui/TextView.h"

namespace NetMan {

/**
 * @struct AgentDiscoveryParams
 */
typedef struct {
    in_addr_t baseIP;
    u16 nhosts;
    u16 port;
    u32 version;
    u32 maxRequests;
    u32 timeout;
    u8 prevProgress;
    u8 progress;
    bool scanning;
    bool error;
} AgentDiscoveryParams;

/**
 * @class AgentDiscoveryController
 */
class AgentDiscoveryController : public GuiController {
    private:
        AgentDiscoveryParams params;
        std::shared_ptr<TextView> progressText;
    public:
        AgentDiscoveryController();
        virtual ~AgentDiscoveryController();
        void initialize(std::vector<std::shared_ptr<GuiView>> &views) override;
        inline AgentDiscoveryParams &getParams() { return params; }
        inline std::shared_ptr<TextView> getProgressText() { return progressText; }
};

}

#endif
