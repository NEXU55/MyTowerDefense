#pragma once
#include"../event/input_event.h"
#include <SDL_rect.h>
#include <memory>
#include <vector>

#include"rect.h"

namespace engine::core
{
    class Context;
}

namespace engine::ui 
{
    /**
     * 所有UI元素的基类
     * 定义了位置、大小、可见性、状态等通用属性。
     * 管理子元素的层次结构。
     * 提供事件处理、更新和渲染的虚方法。
     */
    class UIElement 
    {
    protected:
        using Context = engine::core::Context;
        using InputEvent = engine::event::InputEvent;
    protected:
        glm::dvec2 position_;                                    //相对于父元素的局部位置
        glm::dvec2 size_;                                        //元素大小
        bool visible_ = true;                                   //元素当前是否可见
        bool can_remove_ = false;                              //是否需要移除

        UIElement* parent_ = nullptr;                           //指向父节点的非拥有指针
        std::vector<std::unique_ptr<UIElement>> children_;      //子元素列表(容器)
    public:
        /*
         * 构造UIElement
         * position 初始局部位置
         * size 初始大小
         */
        explicit UIElement(glm::dvec2 position = { 0.0, 0.0 }, glm::dvec2 size = { 0.0, 0.0 });
        virtual ~UIElement() = default;
        // --- 核心虚循环方法 --- (没有使用init和clean，注意构造函数和析构函数的使用)
        virtual bool input(Context& context, const InputEvent& event);
        virtual void update(double delta, Context& context);
        virtual void render(Context& context);

        // --- 层次结构管理 ---
        void add_child(std::unique_ptr<UIElement> child);                //添加子元素
        std::unique_ptr<UIElement> remove_child(UIElement* child_ptr);   //将指定子元素从列表中移除，并返回其智能指针
        void remove_all_children();                                      //移除所有子元素

        // --- Getters and Setters ---
        glm::dvec2 get_size() const { return size_; }              //获取元素大小
        glm::dvec2 get_position() const { return position_; }      //获取元素位置(相对于父节点)
        bool is_visible() const { return visible_; }                     //检查元素是否可见
        bool can_remove() const { return can_remove_; }                  //检查元素是否需要移除
        UIElement* get_parent() const { return parent_; }                //获取父元素
        const std::vector<std::unique_ptr<UIElement>>& get_children() const { return children_; } //获取子元素列表

        void set_size(glm::dvec2 size) { size_ = std::move(size); }      //设置元素大小
        void set_visible(bool visible) { visible_ = visible; }           //设置元素的可见性
        void set_parent(UIElement* parent) { parent_ = parent; }         //设置父节点
        void set_position(glm::vec2 position) { position_ = std::move(position); }   //设置元素位置(相对于父节点)
        void set_can_remove(bool can_remove) { can_remove_ = can_remove; }    //设置元素是否需要移除

        // --- 辅助方法 ---
        Rect get_bound() const;                                          //获取(计算)元素的边界(屏幕坐标)
        glm::dvec2 get_screen_position() const;                          //获取(计算)元素在屏幕上位置
        bool is_point_inside(const glm::dvec2& point) const;             //检查给定点是否在元素的边界内

        // --- 禁用拷贝和移动语义 ---
        UIElement(const UIElement&) = delete;
        UIElement& operator=(const UIElement&) = delete;
        UIElement(UIElement&&) = delete;
        UIElement& operator=(UIElement&&) = delete;
    };
}