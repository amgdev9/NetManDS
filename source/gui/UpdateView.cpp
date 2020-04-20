/**
 * @file UpdateView.cpp
 * @brief View for update callbacks
 */

// Own includes
#include "gui/UpdateView.h"

namespace NetMan {

UpdateView::UpdateView(tinyxml2::XMLElement *node, std::shared_ptr<GuiController> controller) {

    const char *onUpdateStr = node->Attribute("onUpdate");
    if(onUpdateStr != NULL) {
        onUpdate = std::string(onUpdateStr);
    }

    this->controller = controller;
}

void UpdateView::input(u32 held, u32 down, u32 up, touchPosition &touch) {
    if(controller != nullptr && !onUpdate.empty()) {
        UpdateParams params;
        params.controller = controller;
        controller->callMethod(onUpdate, &params);
    }
}

UpdateView::~UpdateView() { }

}
