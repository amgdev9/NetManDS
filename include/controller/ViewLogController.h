/**
 * @file ViewLogController.h
 */

#ifndef _VIEWLOG_CONTROLLER_H_
#define _VIEWLOG_CONTROLLER_H_

// Includes C/C++
#include <memory>

// Includes jansson
#include <jansson.h>

// Own includes
#include "GuiController.h"

namespace NetMan {

/**
 * @class ViewLogController
 */
class ViewLogController : public GuiController {
    private:
        std::shared_ptr<json_t> root;
    public:
        ViewLogController();
        virtual ~ViewLogController();
        inline std::shared_ptr<json_t> getJson() { return root; }
};

}

#endif
