/**
 * @file SnmpController.h
 */

#ifndef _SNMP_CONTROLLER_H_
#define _SNMP_CONTROLLER_H_

// Own includes
#include "GuiController.h"

namespace NetMan {

/**
 * @class SnmpController
 */
class SnmpController : public GuiController {
    public:
        SnmpController();
        virtual ~SnmpController();
};

}

#endif
