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
 * @param name  Name of the image resource
 * @param x     Image x position
 * @param y     Image y position
 * @param sx    Image x scale
 * @param sy    Image y scale
 */
ImageView::ImageView(const char *name, float x, float y, float sx, float sy) {

    this->controller = nullptr;

    // Check image path
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
    C2D_SpriteSetPos(&imageParams, x, y);
    C2D_SpriteSetRotation(&imageParams, 0.0f);
    C2D_SpriteSetScale(&imageParams, sx, sy);
}

/**
 * @brief Load an image view
 * @param node          XML node containing image parameters
 * @param controller    Controller used (can be nullptr)
 */
ImageView::ImageView(XMLElement *node, std::shared_ptr<GuiController> controller) :
    ImageView(node->Attribute("name"), node->FloatAttribute("x", 200.0f), node->FloatAttribute("y", 120.0f),
              node->FloatAttribute("sx", 1.0f), node->FloatAttribute("sy", 1.0f)) {

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
