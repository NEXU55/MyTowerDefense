#pragma once
#include"../event/input_event.h"
#include <memory>
#include <vector>
#include <glm/vec2.hpp>

namespace engine::core 
{
    class Context;
}
namespace engine::ui 
{
    class UIElement;
    class UIPanel; // UIPanel 将作为根元素
}
namespace engine::ui 
{
    /**
     * 管理特定场景中的UI元素集合。
     * 负责UI元素的生命周期管理（通过根元素）、渲染调用和输入事件分发。
     * 每个需要UI的场景（如菜单、游戏HUD）应该拥有一个UIManager实例。
     */
    class UIManager final 
    {
        using Context = engine::core::Context;
        using InputEvent = engine::event::InputEvent;
    private:
        std::unique_ptr<UIPanel> root_element_;                  //一个UIPanel作为根节点(UI元素)
    public:
        UIManager();                                             //构造函数将创建默认的根节点。
        ~UIManager();

        [[nodiscard]] bool init(const glm::dvec2& window_size);  //初始化UI管理器，设置根元素的大小。
        void add_element(std::unique_ptr<UIElement> element);    //添加一个UI元素到根节点的child_容器中。
        UIPanel* get_root_element() const;                       //获取根UIPanel元素的指针。
        void clear();                                            //清除所有UI元素，通常用于重置UI状态。

        // --- 核心循环方法 ---
        bool input(Context& context, const InputEvent& event);                      //处理输入事件，如果事件被处理则返回true。
        void update(double delta, Context&);
        void render(Context&);

        // 禁止拷贝和移动构造/赋值
        UIManager(const UIManager&) = delete;
        UIManager& operator=(const UIManager&) = delete;
        UIManager(UIManager&&) = delete;
        UIManager& operator=(UIManager&&) = delete;
    };
}