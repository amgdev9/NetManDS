/**
 * @file GuiView.h
 * @brief GUI View interface
 */

#ifndef _GUI_VIEW_H_
#define _GUI_VIEW_H_

// Includes 3DS
#include <3ds.h>

namespace NetMan {

/**
 * @class GuiView
 */
class GuiView {
    public:
        virtual void draw() {}
        virtual void input(u32 held, u32 down, u32 up, touchPosition &touch) {}
};

}

#endif
