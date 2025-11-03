#include "ui_pressed_state.h"
#include "ui_normal_state.h"
#include "ui_hover_state.h"
#include "ui_interactive.h"
#include "../core/input_manager.h"
#include "../core/context.h"
#include <spdlog/spdlog.h>

namespace engine::ui{

void UIPressedState::enter()
{
    owner_->set_texture("pressed");
    spdlog::debug("切换到按下状态");
}

std::unique_ptr<UIState> UIPressedState::input(Context& context, const InputEvent& event)
{
    auto& input_manager = context.get_input_manager();
    auto mouse_pos = input_manager.get_mouse_position();
    if (event.message=="MouseLeftClick") 
    {
        if (!owner_->is_point_inside(mouse_pos)) 
        {        
            // 松开鼠标时，如果不在UI元素内，则切换到正常状态
            return std::make_unique<engine::ui::UINormalState>(owner_);
        } 
        else if(event.state==ActionState::RELEASED_THIS_FRAME)
        {         
            owner_->clicked();                               
            // 松开鼠标时，如果还在UI元素内，则触发点击事件
            return std::make_unique<engine::ui::UIHoverState>(owner_);
        }
    }
    return nullptr;
}
}