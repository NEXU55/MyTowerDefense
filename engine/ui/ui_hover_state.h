#pragma once
#include "ui_state.h"

namespace engine::ui
{
/**
 * 悬停状态
 *
 * 当鼠标悬停在UI元素上时，会切换到该状态。
 */
class UIHoverState final: public UIState 
{
    friend class engine::ui::UIInteractive;
public:
    UIHoverState(engine::ui::UIInteractive* owner) 
        : UIState(owner) 
    {}
    ~UIHoverState() override = default;

private:
    void enter() override;
    void exit() override;
    std::unique_ptr<UIState> update(Context& context) override;
    std::unique_ptr<UIState> input(Context& context, const InputEvent& event)override;
};
}