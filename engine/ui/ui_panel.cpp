#include "ui_panel.h"
#include "../core/context.h"
#include "../render/renderer.h"
#include <spdlog/spdlog.h>

namespace engine::ui {

    UIPanel::UIPanel(glm::dvec2 position, glm::dvec2 size, std::optional<SDL_Color> background_color)
        : UIElement(std::move(position), std::move(size)), background_color_(std::move(background_color))
    {
        spdlog::trace("UIPanel 构造完成");
    }

    void UIPanel::render(engine::core::Context& context) 
    {
        if (!visible_) return;
        if (background_color_) 
        {
            context.get_renderer().draw_ui_fill_rect(get_bound(), background_color_.value());
        }
        UIElement::render(context); // 调用基类渲染方法(绘制子节点)
    }

} // namespace engine::ui 