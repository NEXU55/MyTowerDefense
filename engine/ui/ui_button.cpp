#include "ui_button.h"
#include "ui_normal_state.h"
#include <spdlog/spdlog.h>

namespace engine::ui 
{
    UIButton::UIButton(engine::core::Context& context,
        std::string_view normal_sprite_id,
        std::string_view hover_sprite_id,
        std::string_view pressed_sprite_id,
        glm::dvec2 position,
        glm::dvec2 size)
        :UIInteractive(context
        , std::move(position)
        , std::move(size))
    {
        add_texture("normal",normal_sprite_id);
        add_texture("hover", hover_sprite_id);
        add_texture("pressed",pressed_sprite_id);

        // 设置默认状态为"normal"
        set_state(std::make_unique<engine::ui::UINormalState>(this));

        spdlog::trace("UIButton 构造完成");
    }
}