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
 * @struct CheckboxParams
 */
typedef struct {
    bool state;
    bool init;
    std::shared_ptr<GuiController> controller;
} CheckboxParams;

/**
 * @class CheckboxView
 */
class CheckboxView : public ImageView {
    protected:
        std::string onClick;
        C2D_SpriteSheet tickData;
        C2D_Sprite tickParams;
        bool checked;
        void setup();
    public:
        CheckboxView(const char *name, float x, float y, float sx, float sy, std::shared_ptr<GuiController> controller, const std::string &onClick);
        CheckboxView(tinyxml2::XMLElement *node, std::shared_ptr<GuiController> controller);
        void input(u32 held, u32 down, u32 up, touchPosition &touch) override;
        void draw() override;
        virtual ~CheckboxView();
};

}

#endif
