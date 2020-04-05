/**
 * @file ButtonView.h
 * @brief View for buttons
 */

#ifndef _BUTTON_VIEW_H_
#define _BUTTON_VIEW_H_

// Own includes
#include "ImageView.h"

namespace NetMan {

/**
 * @class ButtonView
 */
class ButtonView : public ImageView {
    protected:
        C2D_ImageTint tint;
    public:
        ButtonView(tinyxml2::XMLElement *node, std::shared_ptr<GuiController> controller);
        void draw() override;
        virtual ~ButtonView();
};

}

#endif
