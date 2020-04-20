/**
 * @file UpdateView.h
 * @brief View for update callbacks
 */

#ifndef _UPDATE_VIEW_H_
#define _UPDATE_VIEW_H_

// Includes C/C++
#include <memory>

// Includes tinyxml2
#include <tinyxml2.h>

// Own includes
#include "GuiView.h"
#include "controller/GuiController.h"

namespace NetMan {

/**
 * @struct UpdateParams
 */
typedef struct {
    std::shared_ptr<GuiController> controller;
} UpdateParams;

/**
 * @class UpdateView
 */
class UpdateView : public GuiView {
    protected:
        std::string onUpdate;
        std::shared_ptr<GuiController> controller;
    public:
        UpdateView(tinyxml2::XMLElement *node, std::shared_ptr<GuiController> controller);
        void input(u32 held, u32 down, u32 up, touchPosition &touch) override;
        virtual ~UpdateView();
};

}

#endif
