/**
 * @file AddUserController.h
 */

#ifndef _ADDUSER_CONTROLLER_H_
#define _ADDUSER_CONTROLLER_H_

// Own includes
#include "GuiController.h"
#include "snmp/Snmpv3UserStore.h"

namespace NetMan {

/**
 * @class AddUserController
 */
class AddUserController : public GuiController {
    private:
        Snmpv3UserStoreEntry userEntry;
        std::string username;
    public:
        AddUserController();
        virtual ~AddUserController();
        inline void setUsername(const std::string &username) { this->username = username; }
        inline void setPrivPass(const std::string &privPass) { this->userEntry.privPass = privPass; }
        inline void setAuthPass(const std::string &authPass) { this->userEntry.authPass = authPass; }
        inline void setAuthProto(u32 proto) { this->userEntry.authProto = proto; }
        inline void setPrivProto(u32 proto) { this->userEntry.privProto = proto; }
        inline std::string &getUsername() { return this->username; }
        inline Snmpv3UserStoreEntry &getUserEntry() { return this->userEntry; }
};

}

#endif
