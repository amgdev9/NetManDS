/**
 * @file MenuController.h
 */

#ifndef _MENU_CONTROLLER_H_
#define _MENU_CONTROLLER_H_

// Includes C/C++
#include <memory>

// Own includes
#include "GuiController.h"
#include "WaveAudio.h"

namespace NetMan {

/**
 * @class MenuController
 */
class MenuController : public GuiController {
    private:
        std::unique_ptr<WaveAudio> beepAudio;
    public:
        MenuController();
        virtual ~MenuController();
};

}

#endif
