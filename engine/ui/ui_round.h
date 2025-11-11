#pragma once
#include "ui_element.h"
#include <SDL.h>

namespace engine::ui
{
    /*
     * 一个用于绘制圆形的UI元素。
     * 继承自UIElement并添加了渲染图像的功能。
     */
    class UIRound final : public UIElement
    {
    private:
        SDL_Color color_ = { 255,255,255,255 };
        double range_ = 0;
    public:
        UIRound(glm::dvec2 position, SDL_Color color,double range);
        // --- 核心方法 ---
        void render(engine::core::Context& context) override;

        void set_range(double range) { range_=range; }
    };
}