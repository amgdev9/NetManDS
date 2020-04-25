/**
 * @file SendSnmpController.h
 */

#ifndef _SENDSNMP_CONTROLLER_H_
#define _SENDSNMP_CONTROLLER_H_

// Own includes
#include "GuiController.h"
#include "gui/TextView.h"
#include "gui/ImageView.h"
#include "gui/EditTextView.h"
#include "gui/ListView.h"

namespace NetMan {

/**
 * @struct SendSnmpIcons
 */
typedef struct {
    std::shared_ptr<TextView> oidText;
    std::shared_ptr<ImageView> typeBox;
    std::shared_ptr<TextView> typeText;
    std::shared_ptr<EditTextView> valueEditText;
} SendSnmpIcons;

/**
 * @class SendSnmpController
 */
class SendSnmpController : public GuiController {
    private:
        ListViewFillParams *fillParams;
        std::vector<SendSnmpIcons> sendSnmpIcons;
        u32 currentField;
    public:
        SendSnmpController();
        virtual ~SendSnmpController();
        inline ListViewFillParams *getFillParams() { return fillParams; }
        inline void setFillParams(ListViewFillParams *params) { fillParams = params; }
        inline void clearIcons() { sendSnmpIcons.clear(); }
        void addIcons(std::shared_ptr<TextView> oidText, std::shared_ptr<ImageView> typeBox, std::shared_ptr<TextView> typeText, std::shared_ptr<EditTextView> valueEditText);
        inline std::vector<SendSnmpIcons> &getIcons() { return sendSnmpIcons; }
        inline void setCurrentField(u32 field) { currentField = field; }
        inline u32 getCurrentField() { return currentField; }
};

}

#endif
