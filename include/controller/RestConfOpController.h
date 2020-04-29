/**
 * @file RestConfOpController.h
 */

#ifndef _RESTCONFOP_CONTROLLER_H_
#define _RESTCONFOP_CONTROLLER_H_

// Own includes
#include "GuiController.h"
#include "restconf/YinHelper.h"
#include "tinyxml2.h"

namespace NetMan {

/**
 * @struct RestConfParams
 */
typedef struct {
    tinyxml2::XMLElement *node;
    std::shared_ptr<YinHelper> yinHelper;
    std::string key;
    std::string content;
    std::vector<std::string> fields;
    std::string postData;
    bool isList;
    bool isContainer;
    HTTPC_RequestMethod method;
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
