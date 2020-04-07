/**
 * @file HSlideView.cpp
 * @brief View for horizontal slides
 */

// Includes
#include <stdexcept>

// Own includes
#include "gui/HSlideView.h"
#include "Application.h"

using namespace tinyxml2;

namespace NetMan {

/**
 * @brief Load an HSlide view
 * @param node          XML node containing image parameters
 * @param controller    Controller used (can be nullptr)
 */
HSlideView::HSlideView(XMLElement *node, std::shared_ptr<GuiController> controller) {

    // Initialize data
    currentScreen = 0;

    // Load arrow and flip sound
    try {
        arrow = Application::getInstance().getImageResource("hslideArrow");
    } catch (const std::bad_alloc &e) {
        throw;
    } catch (const std::runtime_error &e) {
        throw;
    }

    // Set up image parameters
    for(u8 i = 0; i < 2; i++) {
        C2D_SpriteFromSheet(&arrowParams[i], arrow, 0);
        C2D_SpriteSetCenter(&arrowParams[i], 0.5f, 0.5f);
        C2D_SpriteSetRotation(&arrowParams[i], 0.0f);
        if(i == ARROW_LEFT) {
            C2D_SpriteSetPos(&arrowParams[i], 16.0f, 160.0f - 16.0f);
            C2D_SpriteSetScale(&arrowParams[i], -1.0f, 1.0f);
        } else {
            C2D_SpriteSetPos(&arrowParams[i], 320.0f - 16.0f, 160.0f - 16.0f);
            C2D_SpriteSetScale(&arrowParams[i], 1.0f, 1.0f);
        }
    }

    // Load each screen
    screens = std::vector<std::shared_ptr<GuiLayout>>();
    for(XMLElement* child = node->FirstChildElement(); child != NULL; child = child->NextSiblingElement()) {
        std::shared_ptr<GuiLayout> screen = std::make_shared<GuiLayout>();
        screen->loadFromNode(child, controller);
        screens.push_back(screen);
    }

    // Save controller
    this->controller = controller;
}

/**
 * @brief Process input for a HSlide view
 * @param held  Held buttons
 * @param down  Down buttons
 * @param up    Up buttons
 * @param touch Touchscreen status
 */
void HSlideView::input(u32 held, u32 down, u32 up, touchPosition &touch) {

    float hw = fabs(arrowParams[0].params.pos.w) / 2.0f;
    float hh = fabs(arrowParams[0].params.pos.h) / 2.0f;

    for(u8 i = 0; i < 2; i++) {
        auto &pos = arrowParams[i].params.pos;

        if(touch.px >= (pos.x - hw) && touch.px <= (pos.x + hw) &&
        touch.py >= (pos.y - hh) && touch.py <= (pos.y + hh) && (down &KEY_TOUCH)) {
            if(i == ARROW_LEFT) {
                if(currentScreen > 0) currentScreen --;
            } else {
                if(currentScreen < (screens.size() - 1)) currentScreen ++;
            }
        }
    }

    screens[currentScreen]->input(held, down, up, touch);
}

/**
 * @brief Draw an HSlide view
 */
void HSlideView::draw() {

    if(currentScreen > 0) C2D_DrawSprite(&arrowParams[ARROW_LEFT]);
    if(currentScreen < (screens.size() - 1)) C2D_DrawSprite(&arrowParams[ARROW_RIGHT]);

    screens[currentScreen]->draw();
}

/**
 * @brief Destructor for an HSlide view
 */
HSlideView::~HSlideView() { }

}
