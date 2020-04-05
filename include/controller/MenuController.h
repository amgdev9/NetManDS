/**
 * @file MenuController.h
 */

#ifndef _MENU_CONTROLLER_H_
#define _MENU_CONTROLLER_H_

// Own includes
#include "GuiController.h"

namespace NetMan {

/**
 * @class MenuController
 */
class MenuController : public GuiController {
    public:
        MenuController();
        virtual ~MenuController();
        void initialize(std::vector<std::shared_ptr<GuiView>> &views) override;
        void callMethod(const std::string &method, void *args) override;
};

}

#endif
