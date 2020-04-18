/**
 * @file HSlideView.h
 * @brief View for horizontal slides
 */

#ifndef _HSLIDE_VIEW_H_
#define _HSLIDE_VIEW_H_

// Includes C/C++
#include <memory>

// Includes 3DS
#include <citro2d.h>

// Includes tinyxml2
#include <tinyxml2.h>

// Own includes
#include "GuiView.h"
#include "controller/GuiController.h"
#include "WaveAudio.h"
#include "GuiLayout.h"

// Defines
#define ARROW_LEFT          0
#define ARROW_RIGHT         1

namespace NetMan {

/**
 * @class HSlideView
 */
class HSlideView : public GuiView {
    protected:
        C2D_SpriteSheet arrow;
        C2D_Sprite arrowParams[2];
        std::shared_ptr<GuiController> controller;
        std::vector<std::shared_ptr<GuiLayout>> screens;
        u32 currentScreen;
        std::string onSetScreen;
    public:
        HSlideView(tinyxml2::XMLElement *node, std::shared_ptr<GuiController> controller);
        void input(u32 held, u32 down, u32 up, touchPosition &touch) override;
        void draw() override;
        virtual ~HSlideView();
};

}

#endif
