#pragma once
#include "ui_interactive.h"
#include <functional>

namespace engine::ui 
{
    /*
     * 按钮UI元素
     * 继承自UIInteractive，用于创建可交互的按钮。
     * 支持三种状态：正常、悬停、按下。
     * 支持回调函数，当按钮被点击时调用。
     */
    class UIButton final : public UIInteractive
    {
    private:
        std::function<void()> click_;
        std::function<void()> hover_;
        std::function<void()> unhover_;
    public:
        /**
         * 构造函数
         * normal_sprite_id 正常状态的精灵ID
         * hover_sprite_id 悬停状态的精灵ID
         * pressed_sprite_id 按下状态的精灵ID
         * position 位置
         * size 大小
         * callback 回调函数
         */
        UIButton(engine::core::Context& context,
            std::string_view normal_sprite_id,
            std::string_view hover_sprite_id,
            std::string_view pressed_sprite_id,
            glm::dvec2 position = { 0.0, 0.0 },
            glm::dvec2 size = { 0.0, 0.0 });
        ~UIButton() override = default;

        void clicked() override     //重写基类方法，当按钮被点击时调用回调函数
        {
            if (click_)
                click_();
        }
        void hovered() override
        {
            if (hover_)
                hover_();
        }

        void unhovered()override
        {
            if (unhover_)
                unhover_();
        }

        void set_click(std::function<void()> click) { click_ = std::move(click); }          //设置点击回调函数
        std::function<void()> get_click() const { return click_; }                          //获取点击回调函数
        void set_hover(std::function<void()> hover) { hover_ = std::move(hover); }          //设置悬浮回调函数
        std::function<void()> get_hover() const { return hover_; }                          //获取悬浮回调函数
        void set_unhover(std::function<void()> unhover) { unhover_ = std::move(unhover); }  //设置取消悬浮回调函数
        std::function<void()> get_unhover() const { return unhover_; }                      //获取取消悬浮回调函数
    };
}