#pragma once
#include "ui_element.h"
#include <optional>
#include <SDL.h>

namespace engine::ui {

    /**
     * 用于分组其他UI元素的容器UI元素
     * Panel通常用于布局和组织。
     * 可以选择是否绘制背景色(纯色)。
     */
    class UIPanel final : public UIElement 
    {
    private:
        std::optional<SDL_Color> background_color_;    //可选背景色
    public:
        /*
         * 构造一个Panel
         * position Panel的局部位置
         * size Panel的大小
         * background_color 背景色
         */
        explicit UIPanel(glm::dvec2 position = { 0.0, 0.0 }, glm::dvec2 size = { 0.0, 0.0 },
            std::optional<SDL_Color> background_color = std::nullopt);

        void setBackgroundColor(std::optional<SDL_Color> background_color) { background_color_ = std::move(background_color); }
        const std::optional<SDL_Color>& getBackgroundColor() const { return background_color_; }
        void render(engine::core::Context& context) override;
    };
}