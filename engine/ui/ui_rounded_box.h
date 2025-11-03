#pragma once
#include "ui_element.h"
#include <SDL.h>

namespace engine::ui
{
    /*
     * 一个用于绘制圆角方条的UI元素。
     * 继承自UIElement并添加了渲染图像的功能。
     */
    class UIRounderBox final : public UIElement
    {
    private:
        SDL_Color color_ = { 255,255,255,255 };
        int rad_ = 0;
    public:
        UIRounderBox(glm::dvec2 position,glm::dvec2 size,SDL_Color color,int rad);
        // --- 核心方法 ---
        void render(engine::core::Context& context) override;
    };
}