/**
 * @file YinBrowserController.h
 */

#ifndef _YINBROWSER_CONTROLLER_H_
#define _YINBROWSER_CONTROLLER_H_

// Includes tinyxml2
#include <tinyxml2.h>

// Own includes
#include "GuiController.h"
#include "gui/ListView.h"
#include "gui/ImageView.h"
#include "restconf/YinHelper.h"

namespace NetMan {

/**
 * @class YinBrowserController
 */
class YinBrowserController : public GuiController {
    private:
        ListViewFillParams *fillParams;
        std::vector<std::shared_ptr<ImageView>> entryIcons;
        std::shared_ptr<YinHelper> yinHelper;
        tinyxml2::XMLElement *currentNode;
    public:
        YinBrowserController();
        virtual ~YinBrowserController();
        inline std::shared_ptr<YinHelper> getYinHelper() { return yinHelper; }
        inline tinyxml2::XMLElement *getCurrentNode() { return currentNode; }
        inline void setCurrentNode(tinyxml2::XMLElement *node) { currentNode = node; }
        inline ListViewFillParams *getFillParams() { return fillParams; }
        inline void setFillParams(ListViewFillParams *params) { fillParams = params; }
        inline void clearIcons() { entryIcons.clear(); }
        inline void addEntryIcons(std::shared_ptr<ImageView> add) { entryIcons.push_back(add); }
        inline std::vector<std::shared_ptr<ImageView>> &getEntryIcons() { return entryIcons; }
        u32 getNodeNChildren(tinyxml2::XMLElement *node);
};

}

#endif
