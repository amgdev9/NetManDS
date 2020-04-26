/**
 * @file SnmpParamsController.h
 */

#ifndef _SNMPPARAMS_CONTROLLER_H_
#define _SNMPPARAMS_CONTROLLER_H_

// Own includes
#include "MibBrowserController.h"
#include "gui/ImageView.h"

namespace NetMan {

/**
 * @struct SnmpThreadParams
 */
typedef struct {
    std::shared_ptr<SnmpSessionParams> session;
    bool working;
    bool error;
    bool success;
    std::string errorText;
    bool usmEnabled;
    std::string community;
    std::string username;
} SnmpThreadParams;

/**
 * @class SnmpParamsController
 */
class SnmpParamsController : public GuiController {
    private:
        SnmpThreadParams threadParams;
        std::shared_ptr<ImageView> loadingIcon;
        std::shared_ptr<ImageView> loadingBox;
    public:
        SnmpParamsController();
        virtual ~SnmpParamsController();
        void initialize(std::vector<std::shared_ptr<GuiView>> &views) override;
        inline bool getUsmEnabled() { return threadParams.usmEnabled; }
        inline std::string getCommunity() { return threadParams.community; }
        inline std::string getUsername() { return threadParams.username; }
        inline void setUsmEnabled(bool usmEnabled) { threadParams.usmEnabled = usmEnabled; }
        inline void setCommunity(const std::string &community) { threadParams.community = community; }
        inline void setUsername(const std::string &username) { threadParams.username = username; }
        inline SnmpThreadParams *getThreadParams() { return &threadParams; }
        inline std::shared_ptr<SnmpSessionParams> getSessionParams() { return threadParams.session; }
        inline std::shared_ptr<ImageView> getLoadingIcon() { return loadingIcon; }
        inline std::shared_ptr<ImageView> getLoadingBox() { return loadingBox; }
};

}

#endif
