/**
 * @file ListView.h
 * @brief View for lists
 */

#ifndef _LIST_VIEW_H_
#define _LIST_VIEW_H_

// Includes C/C++
#include <vector>
#include <memory>

// Includes 3DS
#include <citro2d.h>

// Includes tinyxml2
#include <tinyxml2.h>

// Own includes
#include "GuiLayout.h"
#include "controller/GuiController.h"

namespace NetMan {

/**
 * @struct ListViewFillParams
 */
typedef struct {
    float startX;
    float startY;
    float elementWidth;
    float elementHeight;
    u32 startElement;
    u32 endElement;
    bool remaining;
    std::vector<std::shared_ptr<GuiLayout>> layouts;
} ListViewFillParams;

/**
 * @class ListView
 */
class ListView : public GuiView {
    protected:
        C2D_SpriteSheet arrowData;
        C2D_Sprite arrowParams;
        std::shared_ptr<GuiController> controller;
        u32 maxElements;
        ListViewFillParams fillParams;
        std::string onFill;
        std::string onClick;
    public:
        ListView(tinyxml2::XMLElement *node, std::shared_ptr<GuiController> controller);
        virtual void input(u32 held, u32 down, u32 up, touchPosition &touch) override;
        virtual void draw() override;
        virtual ~ListView();
};

}

#endif
