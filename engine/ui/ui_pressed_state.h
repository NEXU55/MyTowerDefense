#pragma once
#include "ui_state.h"

namespace engine::ui
{

/*
 * 按下状态
 *
 * 当鼠标按下UI元素时，会切换到该状态。
 */
class UIPressedState final: public UIState 
{
    friend class engine::ui::UIInteractive;
public:
    UIPressedState(engine::ui::UIInteractive* owner) 
        : UIState(owner) 
    {}
    ~UIPressedState() override = default;

private:
    void enter() override;
    std::unique_ptr<UIState> update(Context& context) override;
    std::unique_ptr<UIState> input(Context& context, const InputEvent& event) override;
};
}