#pragma once
#include "ui_state.h"

namespace engine::ui 
{
/*
 * 正常状态
 * 正常状态是UI元素的默认状态。
 */
class UINormalState final: public UIState {
    friend class engine::ui::UIInteractive;
public:
    UINormalState(engine::ui::UIInteractive* owner) 
        : UIState(owner) 
    {}
    ~UINormalState() override = default;

private:
    void enter() override;
    std::unique_ptr<UIState> update(Context& context) override;
    std::unique_ptr<UIState> input(Context& context, const InputEvent& event) override
    {
        return nullptr;
    }
};
}