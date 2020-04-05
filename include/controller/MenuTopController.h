/**
 * @file MenuTopController.h
 */

#ifndef _MENU_TOPCONTROLLER_H_
#define _MENU_TOPCONTROLLER_H_

// Own includes
#include "GuiController.h"

namespace NetMan {

/**
 * @class MenuTopController
 */
class MenuTopController : public GuiController {
    public:
        MenuTopController();
        virtual ~MenuTopController();
        void initialize(std::vector<std::shared_ptr<GuiView>> &views) override;
        void callMethod(const std::string &method, void *args) override;
};

}

#endif
