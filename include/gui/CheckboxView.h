/**
 * @file CheckboxView.h
 * @brief View for checkboxes
 */

#ifndef _CHECKBOX_VIEW_H_
#define _CHECKBOX_VIEW_H_

// Own includes
#include "ImageView.h"

namespace NetMan {

/**
 * @class CheckboxView
 */
class CheckboxView : public ImageView {
    protected:
        std::string onClick;
        C2D_SpriteSheet tickData;
        C2D_Sprite tickParams;
        bool checked;
    public:
        CheckboxView(tinyxml2::XMLElement *node, std::shared_ptr<GuiController> controller);
        void input(u32 held, u32 down, u32 up, touchPosition &touch) override;
        void draw() override;
        virtual ~CheckboxView();
};

}

#endif
