/**
 * @file MibBrowserController.h
 */

#ifndef _MIBBROWSER_CONTROLLER_H_
#define _MIBBROWSER_CONTROLLER_H_

// Includes C/C++
#include <memory>
#include <vector>

// Own includes
#include "GuiController.h"
#include "snmp/MibLoader.h"
#include "gui/ListView.h"
#include "gui/ImageView.h"
#include "gui/TextView.h"

namespace NetMan {

/**
 * @struct MibEntryIcons
 */
typedef struct {
    std::shared_ptr<TextView> text;
    std::shared_ptr<ImageView> tick;
    std::shared_ptr<ImageView> add;
    std::shared_ptr<ImageView> table;
} MibEntryIcons;

/**
 * @class MibBrowserController
 */
class MibBrowserController : public GuiController {
    private:
        std::shared_ptr<Mib> mib;
        std::shared_ptr<MibOid> currentTree;
        ListViewFillParams *fillParams;
        std::vector<MibEntryIcons> entryIcons;
    public:
        MibBrowserController();
        virtual ~MibBrowserController();
        inline std::shared_ptr<MibOid> getCurrentTree() { return currentTree; }
        inline void setCurrentTree(std::shared_ptr<MibOid> tree) { currentTree = tree; }
        inline std::shared_ptr<Mib> getMib() { return mib; }
        inline ListViewFillParams *getFillParams() { return fillParams; }
        inline void setFillParams(ListViewFillParams *params) { fillParams = params; }
        inline void clearIcons() { entryIcons.clear(); }
        void addEntryIcons(std::shared_ptr<TextView> text, std::shared_ptr<ImageView> tick, std::shared_ptr<ImageView> add, std::shared_ptr<ImageView> table);
        inline std::vector<MibEntryIcons> &getEntryIcons() { return entryIcons; }
};

}

#endif
