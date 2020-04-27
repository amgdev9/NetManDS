/**
 * @file SnmpTableController.h
 */

#ifndef _SNMPTABLE_CONTROLLER_H_
#define _SNMPTABLE_CONTROLLER_H_

// Own includes
#include "SnmpParamsController.h"
#include "gui/EditTextView.h"
#include "gui/ListView.h"

namespace NetMan {

/**
 * @struct SnmpTableIcons
 */
typedef struct {
    std::shared_ptr<TextView> columnText;
    std::shared_ptr<ImageView> typeBox;
    std::shared_ptr<TextView> typeText;
    std::shared_ptr<EditTextView> valueEditText;
} SnmpTableIcons;

/**
 * @class SnmpTableController
 */
class SnmpTableController : public GuiController {
    private:
        static std::shared_ptr<SnmpThreadParams> snmpParams;
        ListViewFillParams *fillParams;
        std::vector<SnmpTableIcons> tableIcons;
        u32 tableIndex;
        u32 currentField;
    public:
        SnmpTableController();
        inline std::shared_ptr<SnmpThreadParams> getSnmpParams() { return snmpParams; }
        inline void resetSnmpParams() { snmpParams = nullptr; }
        virtual ~SnmpTableController();
        inline ListViewFillParams *getFillParams() { return fillParams; }
        inline void setFillParams(ListViewFillParams *params) { fillParams = params; }
        inline void clearIcons() { tableIcons.clear(); }
        void addIcons(std::shared_ptr<TextView> columnText, std::shared_ptr<ImageView> typeBox, std::shared_ptr<TextView> typeText, std::shared_ptr<EditTextView> valueEditText);
        inline std::vector<SnmpTableIcons> &getIcons() { return tableIcons; }
        inline void setCurrentField(u32 field) { currentField = field; }
        inline u32 getCurrentField() { return currentField; }
        inline u32 *getTableIndex() { return &tableIndex; }
};

}

#endif
