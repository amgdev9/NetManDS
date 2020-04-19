/**
 * @file LogsController.h
 */

#ifndef _LOGS_CONTROLLER_H_
#define _LOGS_CONTROLLER_H_

// Own includes
#include "GuiController.h"

namespace NetMan {

/**
 * @class LogsController
 */
class LogsController : public GuiController {
    public:
        LogsController();
        virtual ~LogsController();
};

}

#endif
