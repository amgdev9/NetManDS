/**
 * @file SshController.h
 */

#ifndef _SSH_CONTROLLER_H_
#define _SSH_CONTROLLER_H_

// Own includes
#include "GuiController.h"

namespace NetMan {

/**
 * @class SshController
 */
class SshController : public GuiController {
    public:
        SshController();
        virtual ~SshController();
};

}

#endif
