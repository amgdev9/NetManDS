/**
 * @file SshController.h
 */

#ifndef _SSH_CONTROLLER_H_
#define _SSH_CONTROLLER_H_

// Own includes
#include "GuiController.h"

namespace NetMan {

/**
 * @struct SshSession
 */
typedef struct {
    std::string host;
    u16 port;
    std::string username;
    std::string password;
} SshSession;

/**
 * @class SshController
 */
class SshController : public GuiController {
    private:
        SshSession session;
    public:
        SshController();
        inline SshSession &getSession() { return session; }
        virtual ~SshController();
};

}

#endif
