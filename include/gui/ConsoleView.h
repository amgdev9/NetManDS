/**
 * @file ConsoleView.h
 * @brief View for text consoles
 */

#ifndef _CONSOLE_VIEW_H_
#define _CONSOLE_VIEW_H_

// Includes C/C++
#include <memory>

// Includes 3DS
#include <citro2d.h>

// Includes tinyxml2
#include <tinyxml2.h>

// Own includes
#include "GuiView.h"
#include "controller/GuiController.h"

namespace NetMan {

/**
 * @class ConsoleView
 */
class ConsoleView : public GuiView {
    private:
        C2D_SpriteSheet fontData;
        C2D_Sprite fontParams;
        std::shared_ptr<GuiController> controller;
    public:
        ConsoleView(tinyxml2::XMLElement *node, std::shared_ptr<GuiController> controller);
        void input(u32 held, u32 down, u32 up, touchPosition &touch) override;
        void draw() override;
        virtual ~ConsoleView();
};

}

#endif
