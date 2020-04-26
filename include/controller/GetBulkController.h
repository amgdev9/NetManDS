/**
 * @file GetBulkController.h
 */

#ifndef _GETBULK_CONTROLLER_H_
#define _GETBULK_CONTROLLER_H_

// Includes C/C++
#include <memory>

// Own includes
#include "MibBrowserController.h"

namespace NetMan {

/**
 * @class GetBulkController
 */
class GetBulkController : public GuiController {
    private:
        std::shared_ptr<SnmpSessionParams> sessionParams;
    public:
        GetBulkController();
        virtual ~GetBulkController();
        inline std::shared_ptr<SnmpSessionParams> getSessionParams() { return sessionParams; }
};

}

#endif
