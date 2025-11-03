#include "ui_rounded_box.h"
#include "../render/renderer.h"
#include "../core/context.h"
#include <SDL2_gfxPrimitives.h>

namespace engine::ui {

    UIRounderBox::UIRounderBox(glm::dvec2 position,glm::dvec2 size,SDL_Color color,int rad)
        : UIElement(std::move(position), std::move(size))
        ,color_(color)
        ,rad_(rad)
    {}

    void UIRounderBox::render(engine::core::Context& context)
    {
        if (!visible_)
            return; 
        // 渲染自身
        auto position = get_screen_position();
        roundedBoxRGBA(context.get_renderer().get_sdl_renderer(),position.x,position.y , position.x +size_.x,position.y+size_.y , rad_ ,
            color_.r, color_.g, color_.b, color_.a);
        // 渲染子元素（调用基类方法）
        UIElement::render(context);
    }

} // namespace engine::ui 