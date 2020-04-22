/**
 * @file AgentViewController.h
 */

#ifndef _AGENTVIEW_CONTROLLER_H_
#define _AGENTVIEW_CONTROLLER_H_

// Includes C/C++
#include <memory>

// Includes jansson
#include <jansson.h>

// Own includes
#include "GuiController.h"

namespace NetMan {

/**
 * @class AgentViewController
 */
class AgentViewController : public GuiController {
    private:
        std::shared_ptr<json_t> root;
    public:
        AgentViewController();
        virtual ~AgentViewController();
        inline std::shared_ptr<json_t> getJson() { return root; }
};

}

#endif
