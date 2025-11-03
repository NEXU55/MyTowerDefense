#include "ui_image.h"
#include "../render/renderer.h"
#include "../resource/resource_manager.h"
#include "../core/context.h"
#include <spdlog/spdlog.h>

namespace engine::ui {

    UIImage::UIImage(SDL_Texture* texture,glm::dvec2 position,
        glm::dvec2 size,SDL_Rect src_rect)
        : UIElement(std::move(position), std::move(size))
        ,texture_(texture)
        ,src_rect_(src_rect)
    {
        if (!texture_)
            spdlog::warn("创建了一个空纹理的UIImage");
        spdlog::trace("UIImage 构造完成");
    }

    void UIImage::render(engine::core::Context& context) 
    {
        if (!visible_ || !texture_)
            return; // 如果不可见或没有分配纹理则不渲染

        // 渲染自身
        auto position = get_screen_position();
        context.get_renderer().draw_sprite(texture_,src_rect_, position,0);
        // 渲染子元素（调用基类方法）
        UIElement::render(context);
    }

} // namespace engine::ui 