/**
 * @file ImageView.h
 * @brief View for images
 */

#ifndef _IMAGE_VIEW_H_
#define _IMAGE_VIEW_H_

// Includes 3DS
#include <citro2d.h>

// Includes tinyxml2
#include <tinyxml2.h>

// Own includes
#include "GuiView.h"

namespace NetMan {

/**
 * @class ImageView
 */
class ImageView : public GuiView {
    private:
        C2D_SpriteSheet imageData;
        C2D_Sprite imageParams;
    public:
        ImageView(tinyxml2::XMLElement *node);
        void draw() override;
        virtual ~ImageView();
};

}

#endif
