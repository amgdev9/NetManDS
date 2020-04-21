/**
 * @file LogsController.h
 */

#ifndef _LOGS_CONTROLLER_H_
#define _LOGS_CONTROLLER_H_

// Includes C/C++
#include <memory>

// Includes jansson
#include <jansson.h>

// Own includes
#include "GuiController.h"
#include "gui/ListView.h"

namespace NetMan {

/**
 * @class LogsController
 */
class LogsController : public GuiController {
    private:
        std::shared_ptr<json_t> root;
        ListViewFillParams *fillParams;
    public:
        LogsController();
        virtual ~LogsController();
        void loadJsonList(const std::string &path);
        inline std::shared_ptr<json_t> getJson() { return root; }
        inline void setFillParams(ListViewFillParams *params) { fillParams = params; }
        inline ListViewFillParams *getFillParams() { return fillParams; }
};

}

#endif
