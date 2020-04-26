/**
 * @file LoadYinController.h
 */

#ifndef _LOADYIN_CONTROLLER_H_
#define _LOADYIN_CONTROLLER_H_

// Includes C/C++
#include <string>
#include <vector>

// Own includes
#include "GuiController.h"

namespace NetMan {

/**
 * @class LoadYinController
 */
class LoadYinController : public GuiController {
    private:
        std::vector<std::string> dirEntries;
    public:
        LoadYinController();
        std::vector<std::string> &getDirEntries() { return dirEntries; }
        virtual ~LoadYinController();
};

}

#endif
