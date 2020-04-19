/**
 * @file SshConsoleController.h
 */

#ifndef _SSHCONSOLE_CONTROLLER_H_
#define _SSHCONSOLE_CONTROLLER_H_

// Own includes
#include "GuiController.h"
#include "ssh/SshHelper.h"

namespace NetMan {

/**
 * @class SshConsoleController
 */
class SshConsoleController : public GuiController {
    private:
        std::shared_ptr<SshHelper> helper;
    public:
        SshConsoleController();
        inline std::shared_ptr<SshHelper> getHelper() { return helper; }
        virtual ~SshConsoleController();
};

}

#endif
