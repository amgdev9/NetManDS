/**
 * @file BinaryButtonView.h
 * @brief View for binary buttons
 */

#ifndef _BINARYBUTTON_VIEW_H_
#define _BINARYBUTTON_VIEW_H_

// Own includes
#include "ImageView.h"

namespace NetMan {

/**
 * @class BinaryButtonView
 */
class BinaryButtonView : public ImageView {
    protected:
        C2D_ImageTint tints[2];
        bool selected;
        std::string onClick;
    public:
        BinaryButtonView(tinyxml2::XMLElement *node, std::shared_ptr<GuiController> controller);
        void input(u32 held, u32 down, u32 up, touchPosition &touch) override;
        void draw() override;
        virtual ~BinaryButtonView();
};

}

#endif
