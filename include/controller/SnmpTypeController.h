/**
 * @file SnmpTypeController.h
 */

#ifndef _SNMPTYPE_CONTROLLER_H_
#define _SNMPTYPE_CONTROLLER_H_

// Own includes
#include "GuiController.h"

namespace NetMan {

/**
 * @class SnmpTypeController
 */
class SnmpTypeController : public GuiController {
    private:
        u32 field;
    public:
        SnmpTypeController();
        virtual ~SnmpTypeController();
        inline u32 getField() { return field; }
};

}

#endif
