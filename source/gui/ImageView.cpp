/**
 * @file ImageView.cpp
 * @brief View for images
 */

// Includes
#include <stdexcept>

// Own includes
#include "gui/ImageView.h"
#include "Application.h"

using namespace tinyxml2;

namespace NetMan {

/**
 * @brief Load an image view
 * @param node          XML node containing image parameters
 * @param controller    Controller used (can be nullptr)
 */
ImageView::ImageView(XMLElement *node, std::shared_ptr<GuiController> controller) {

    // Get image path
    const char *name = node->Attribute("name");
    if(name == NULL) {
        throw std::runtime_error("No image path");
    }

    // Load image data
    try {
        imageData = Application::getInstance().getImageResource(name);
    } catch (const std::runtime_error &e) {
        throw;
    }

    // Set up image parameters
    C2D_SpriteFromSheet(&imageParams, imageData, 0);
    C2D_SpriteSetCenter(&imageParams, 0.5f, 0.5f);
    C2D_SpriteSetPos(&imageParams, node->FloatAttribute("x", 200.0f), node->FloatAttribute("y", 120.0f));
    C2D_SpriteSetRotation(&imageParams, 0.0f);

    // Save controller
    this->controller = controller;
}

/**
 * @brief Draw an image view
 */
void ImageView::draw() {
    C2D_DrawSprite(&imageParams);
}

/**
 * @brief Destructor for an image view
 */
ImageView::~ImageView() { }

}
