#include "ui_normal_state.h"
#include "ui_hover_state.h"
#include "ui_interactive.h"
#include "../core/input_manager.h"
#include "../core/context.h"
#include "../audio/audio_player.h"
#include <spdlog/spdlog.h>

namespace engine::ui
{
void UINormalState::enter()
{
    owner_->set_texture("normal");
    spdlog::debug("切换到正常状态");
}

std::unique_ptr<UIState> UINormalState::update(Context& context)
{
    auto& input_manager = context.get_input_manager();
    auto mouse_pos = input_manager.get_mouse_position();
    if (owner_->is_point_inside(mouse_pos)) 
    {         // 如果鼠标在UI元素内，则切换到悬停状态
        return std::make_unique<engine::ui::UIHoverState>(owner_);
    }
    return nullptr;
}
} // namespace engine::ui::state