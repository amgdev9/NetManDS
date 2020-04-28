/**
 * @file RestConfOpController.h
 */

#ifndef _RESTCONFOP_CONTROLLER_H_
#define _RESTCONFOP_CONTROLLER_H_

// Own includes
#include "GuiController.h"

namespace NetMan {

/**
 * @struct RestConfParams
 */
typedef struct {
    std::string url;
    bool isList;
} RestConfParams;

/**
 * @class RestConfOpController
 */
class RestConfOpController : public GuiController {
    private:
        std::shared_ptr<RestConfParams> restConfParams;
    public:
        RestConfOpController();
        virtual ~RestConfOpController();
        inline std::shared_ptr<RestConfParams> getRestConfParams() { return restConfParams; }
};

}

#endif
