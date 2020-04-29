/**
 * @file RestConfKeyController.h
 */

#ifndef _RESTCONFKEY_CONTROLLER_H_
#define _RESTCONFKEY_CONTROLLER_H_

// Own includes
#include "RestConfOpController.h"

namespace NetMan {

/**
 * @class RestConfKeyController
 */
class RestConfKeyController : public GuiController {
    private:
        std::shared_ptr<RestConfParams> rcParams;
    public:
        RestConfKeyController();
        virtual ~RestConfKeyController();
        inline std::shared_ptr<RestConfParams> getRestConfParams() { return rcParams; }
};

}

#endif
