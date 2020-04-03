/**
 * @file GuiController.h
 * @brief GUI Controller interface
 */

#ifndef _GUICONTROLLER_H_
#define _GUICONTROLLER_H_

// Includes C/C++
#include <string>

namespace NetMan {

class GuiController {
	public:
        virtual void callMethod(const std::string &method) = 0;
};

}

#endif
