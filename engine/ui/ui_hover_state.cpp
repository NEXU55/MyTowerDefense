#include "ui_hover_state.h"
#include "ui_normal_state.h"
#include "ui_pressed_state.h"
#include "ui_interactive.h"
#include "../core/input_manager.h"
#include "../core/context.h"
#include <spdlog/spdlog.h>

namespace engine::ui
{
void UIHoverState::enter()
{
    owner_->set_texture("hover");
    owner_->hovered();
    spdlog::debug("切换到悬停状态");
}

void UIHoverState::exit()
{
    owner_->unhovered();
}

std::unique_ptr<UIState> UIHoverState::input(Context& context, const InputEvent& event)
{
    auto& input_manager = context.get_input_manager();
    auto mouse_pos = input_manager.get_mouse_position();
    if (!owner_->is_point_inside(mouse_pos))// 如果鼠标不在UI元素内，则返回正常状态
        return std::make_unique<UINormalState>(owner_);
    if (event.message == "MouseLeftClick"&&event.state==ActionState::PRESSED_THIS_FRAME)// 如果鼠标按下，则返回按下状态
        return std::make_unique<UIPressedState>(owner_);
    return nullptr;
}
} 