/**
 * @file OptionsController.h
 */

#ifndef _OPTIONS_CONTROLLER_H_
#define _OPTIONS_CONTROLLER_H_

// Includes C/C++
#include <vector>
#include <memory>

// Own includes
#include "GuiController.h"
#include "gui/ImageView.h"
#include "gui/TextView.h"

namespace NetMan {

/**
 * @class OptionsController
 */
class OptionsController : public GuiController {
    private:
        std::vector<std::shared_ptr<ImageView>> crosses;
        std::vector<std::shared_ptr<TextView>> texts;
    public:
        OptionsController();
        inline void clearCrosses() { crosses.clear(); }
        inline void addCross(std::shared_ptr<ImageView> cross) { crosses.push_back(cross); }
        inline std::vector<std::shared_ptr<ImageView>> &getCrosses() { return crosses; }
        inline void clearTexts() { texts.clear(); }
        inline void addText(std::shared_ptr<TextView> text) { texts.push_back(text); }
        inline std::vector<std::shared_ptr<TextView>> &getTexts() { return texts; }
        virtual ~OptionsController();
};

}

#endif
