/**
 * @file SnmpParamsController.h
 */

#ifndef _SNMPPARAMS_CONTROLLER_H_
#define _SNMPPARAMS_CONTROLLER_H_

// Own includes
#include "MibBrowserController.h"

namespace NetMan {

/**
 * @class SnmpParamsController
 */
class SnmpParamsController : public GuiController {
    private:
        std::shared_ptr<SnmpSessionParams> snmpSessionParams;
        bool usmEnabled;
        std::string community;
        std::string username;
    public:
        SnmpParamsController();
        virtual ~SnmpParamsController();
        inline bool getUsmEnabled() { return usmEnabled; }
        inline std::string getCommunity() { return community; }
        inline std::string getUsername() { return username; }
        inline void setUsmEnabled(bool usmEnabled) { this->usmEnabled = usmEnabled; }
        inline void setCommunity(const std::string &community) { this->community = community; }
        inline void setUsername(const std::string &username) { this->username = username; }
};

}

#endif
