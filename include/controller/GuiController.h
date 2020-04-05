/**
 * @file GuiController.h
 * @brief GUI Controller interface
 */

#ifndef _GUICONTROLLER_H_
#define _GUICONTROLLER_H_

// Includes C/C++
#include <string>
#include <memory>
#include <vector>

// Own includes
#include "gui/GuiView.h"

namespace NetMan {

class GuiController {
	public:
        static std::shared_ptr<GuiController> createController(const std::string &className);
        virtual void initialize(std::vector<std::shared_ptr<GuiView>> &views) { }
        virtual void callMethod(const std::string &method, void *args) = 0;
};

}

#endif
