#include "ui_round.h"
#include "../render/renderer.h"
#include "../core/context.h"
#include <SDL2_gfxPrimitives.h>

namespace engine::ui {

    UIRound::UIRound(glm::dvec2 position,SDL_Color color, double range)
        : UIElement(std::move(position), glm::dvec2{0,0})
        , color_(color)
        , range_(range)
    {
    }

    void UIRound::render(engine::core::Context& context)
    {
        if (!visible_)
            return;
        // 渲染自身
        auto position = get_screen_position();
        if (range_ > 0)
        {
            filledCircleRGBA(context.get_renderer().get_sdl_renderer(), position.x, position.y,range_,
                color_.r, color_.g, color_.b, color_.a);
        }
        UIElement::render(context);
    }
} // namespace engine::ui 