/**
 * @file SnmpTypeController.h
 */

#ifndef _SNMPTYPE_CONTROLLER_H_
#define _SNMPTYPE_CONTROLLER_H_

// Own includes
#include "GuiController.h"

namespace NetMan {

/**
 * @struct SnmpTypeContext
 */
typedef struct {
    u32 element;
    bool comesFromTable;
} SnmpTypeContext;

/**
 * @class SnmpTypeController
 */
class SnmpTypeController : public GuiController {
    private:
        std::shared_ptr<SnmpTypeContext> context;
    public:
        SnmpTypeController();
        virtual ~SnmpTypeController();
        inline std::shared_ptr<SnmpTypeContext> getContext() { return context; }
};

}

#endif
