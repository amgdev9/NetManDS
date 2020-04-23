/**
 * @file SetSMIController.h
 */

#ifndef _SETSMI_CONTROLLER_H_
#define _SETSMI_CONTROLLER_H_

// Includes C/C++
#include <vector>

// Own includes
#include "GuiController.h"

namespace NetMan {

/**
 * @class SetSMIController
 */
class SetSMIController : public GuiController {
    private:
        std::vector<std::string> dirEntries;
        bool comesFromOptions;
    public:
        SetSMIController();
        std::vector<std::string> &getDirEntries() { return dirEntries; }
        virtual ~SetSMIController();
        inline bool getComesFromOptions() { return comesFromOptions; }
};

}

#endif
