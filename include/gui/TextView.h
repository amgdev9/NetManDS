/**
 * @file TextView.h
 * @brief View for text
 */

#ifndef _TEXT_VIEW_H_
#define _TEXT_VIEW_H_

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
 * @class TextView
 */
class TextView : public GuiView {
    protected:
        C2D_TextBuf textBuffer;
        C2D_Text text;
        float x, y, scale;
        u32 color;
        std::shared_ptr<GuiController> controller;
        std::string textString;
    public:
        TextView(const std::string &textValue, float x, float y, float scale, u32 color);
        TextView(const std::string &textValue, std::shared_ptr<TextView> textView);
        TextView(tinyxml2::XMLElement *node, std::shared_ptr<GuiController> controller);
        inline void setX(float x) { this->x = x; }
        inline float getX() { return x; }
        inline std::string &getText() { return textString; }
        void setText(const std::string &textValue);
        virtual void draw() override;
        virtual ~TextView();
};

}

#endif
