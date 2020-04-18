/**
 * @file SshConsoleController.h
 */

#ifndef _SSHCONSOLE_CONTROLLER_H_
#define _SSHCONSOLE_CONTROLLER_H_

// Own includes
#include "GuiController.h"

namespace NetMan {

/**
 * @class SshConsoleController
 */
class SshConsoleController : public GuiController {
    private:
    public:
        SshConsoleController();
        virtual ~SshConsoleController();
};

}

#endif
