/**
 * @file MenuController.h
 */

#ifndef _MENU_CONTROLLER_H_
#define _MENU_CONTROLLER_H_

// Includes C/C++
#include <memory>

// Own includes
#include "GuiController.h"
#include "socket/UdpSocket.h"
#include "socket/TcpSocket.h"

namespace NetMan {

/**
 * @class MenuController
 */
class MenuController : public GuiController {
    private:
    public:
        MenuController();
        virtual ~MenuController();
};

}

#endif
