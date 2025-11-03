#pragma once
#include"../event/input_event.h"
#include <memory>

namespace engine::core 
{
    class Context;
}
namespace engine::ui 
{
    class UIInteractive;
}

namespace engine::ui
{
    /*
     * 可交互UI元素在特定状态下的行为接口。
     * 该接口定义了所有具体UI状态必须实现的通用操作，
     * 例如处理输入事件、更新状态逻辑以及确定视觉表现。
     */
    class UIState 
    {
        friend class engine::ui::UIInteractive;
    protected:
        using Context = engine::core::Context;
        using InputEvent = engine::event::InputEvent;
    protected:
        engine::ui::UIInteractive* owner_ = nullptr;    //指向父节点
    public:
        /*
         * @brief 构造函数传入父节点指针
         */
        UIState(engine::ui::UIInteractive* owner) 
            : owner_(owner)
        {}
        virtual ~UIState() = default;

        // 删除拷贝和移动构造函数/赋值运算符
        UIState(const UIState&) = delete;
        UIState& operator=(const UIState&) = delete;
        UIState(UIState&&) = delete;
        UIState& operator=(UIState&&) = delete;

    protected:
        // --- 核心方法 --- 
        virtual void enter() {}
        virtual void exit() {}
        virtual std::unique_ptr<UIState> input(Context& context,const InputEvent& event) = 0;
    };
}