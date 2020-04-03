/**
 * @file GuiView.h
 * @brief GUI View interface
 */

#ifndef _GUI_VIEW_H_
#define _GUI_VIEW_H_

namespace NetMan {

/**
 * @class GuiView
 */
class GuiView {
    public:
        virtual void draw() = 0;
};

}

#endif
