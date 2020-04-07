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
 * @struct ButtonParams
 */
typedef struct {
    std::shared_ptr<GuiController> controller;
} ButtonParams;

/**
 * @class ButtonView
 */
class ButtonView : public ImageView {
    protected:
        C2D_ImageTint tint;
        std::string onClick;
    public:
        ButtonView(tinyxml2::XMLElement *node, std::shared_ptr<GuiController> controller);
        void input(u32 held, u32 down, u32 up, touchPosition &touch) override;
        void draw() override;
        virtual ~ButtonView();
};

}

#endif
