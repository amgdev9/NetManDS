/**
 * @file ImageView.h
 * @brief View for images
 */

#ifndef _IMAGE_VIEW_H_
#define _IMAGE_VIEW_H_

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
 * @class ImageView
 */
class ImageView : public GuiView {
    protected:
        C2D_SpriteSheet imageData;
        C2D_Sprite imageParams;
        std::shared_ptr<GuiController> controller;
    public:
        ImageView(const char *name, float x, float y, float sx, float sy);
        ImageView(tinyxml2::XMLElement *node, std::shared_ptr<GuiController> controller);
        bool touched(u32 down, touchPosition &touch);
        virtual void draw() override;
        virtual ~ImageView();
};

}

#endif
