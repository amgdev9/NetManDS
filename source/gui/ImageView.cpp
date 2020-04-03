/**
 * @file ImageView.cpp
 * @brief View for images
 */

// Includes
#include <stdexcept>

// Own includes
#include "gui/ImageView.h"

using namespace tinyxml2;

namespace NetMan {

/**
 * @brief Load an image view
 * @param node  XML node containing image parameters
 */
ImageView::ImageView(XMLElement *node) {

    // Get image path
    const char *name = node->Attribute("name");
    if(name == NULL) {
        throw std::runtime_error("No image path");
    }
    
    std::string path = std::string("romfs:/gfx/") + name + ".t3x";

    // Load image data
    imageData = C2D_SpriteSheetLoad(path.c_str());
    if(imageData == NULL) {
        throw std::runtime_error("Can't load: " + path);
    }

    // Set up image parameters
    C2D_SpriteFromSheet(&imageParams, imageData, 0);
    C2D_SpriteSetCenter(&imageParams, 0.5f, 0.5f);
    C2D_SpriteSetPos(&imageParams, node->FloatAttribute("x", 200.0f), node->FloatAttribute("y", 120.0f));
    C2D_SpriteSetRotation(&imageParams, 0.0f);
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
ImageView::~ImageView() {
    C2D_SpriteSheetFree(imageData);
}

}
