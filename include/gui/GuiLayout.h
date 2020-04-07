/**
 * @file GuiLayout.h
 * @brief GUI layout
 */

#ifndef _GUILAYOUT_H_
#define _GUILAYOUT_H_

// Includes C/C++
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

// Includes 3DS
#include <3ds/types.h>

// Includes tinyxml2
#include <tinyxml2.h>

// Own includes
#include "GuiView.h"
#include "controller/GuiController.h"

namespace NetMan {

class GuiLayout {
    private:
        std::vector<std::shared_ptr<GuiView>> views;
        std::shared_ptr<GuiController> controller;
	public:
        void loadFromNode(tinyxml2::XMLElement *root, std::shared_ptr<GuiController> controller = nullptr);
        GuiLayout();
        GuiLayout(const std::string &path);
        virtual ~GuiLayout();
        void input(u32 held, u32 down, u32 up, touchPosition &touch);
		void draw();
};

}

#endif
