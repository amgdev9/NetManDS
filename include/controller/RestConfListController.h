/**
 * @file RestConfListController.h
 */

#ifndef _RESTCONFLIST_CONTROLLER_H_
#define _RESTCONFLIST_CONTROLLER_H_

// Own includes
#include "RestConfOpController.h"
#include "gui/CheckboxView.h"

namespace NetMan {

/**
 * @class RestConfListController
 */
class RestConfListController : public GuiController {
    private:
        std::shared_ptr<RestConfParams> rcParams;
        std::vector<std::shared_ptr<CheckboxView>> entryIcons;
        std::vector<std::string> listElements;
        std::shared_ptr<bool> listState;
        u32 currentElement;
    public:
        RestConfListController();
        virtual ~RestConfListController();
        inline std::shared_ptr<RestConfParams> getRestConfParams() { return rcParams; }
        inline void clearIcons() { entryIcons.clear(); }
        inline void addEntryIcons(std::shared_ptr<CheckboxView> checkbox) { entryIcons.push_back(checkbox); }
        inline std::vector<std::shared_ptr<CheckboxView>> &getEntryIcons() { return entryIcons; }
        inline std::vector<std::string> &getListElements() { return listElements; }
        inline void setCurrentElement(u32 element) { currentElement = element; }
        inline u32 getCurrentElement() { return currentElement; }
        inline std::shared_ptr<bool> getListState() { return listState; }
};

}

#endif
