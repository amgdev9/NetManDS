/**
 * @file AgentDiscoveryController.h
 */

#ifndef _AGENTDISCOVERY_CONTROLLER_H_
#define _AGENTDISCOVERY_CONTROLLER_H_

// Own includes
#include "GuiController.h"

namespace NetMan {

/**
 * @struct AgentDiscoveryParams
 */
typedef struct {
    in_addr_t baseIP;
    u16 nhosts;
    u16 port;
    u8 version;
    u8 maxRequests;
    u8 timeout;
} AgentDiscoveryParams;

/**
 * @class AgentDiscoveryController
 */
class AgentDiscoveryController : public GuiController {
    private:
        AgentDiscoveryParams params;
    public:
        AgentDiscoveryController();
        virtual ~AgentDiscoveryController();
        inline AgentDiscoveryParams &getParams() { return params; }
};

}

#endif
