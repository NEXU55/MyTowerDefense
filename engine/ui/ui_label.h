#pragma once
#include "ui_element.h"
#include "../render/text_renderer.h"
#include <string>
#include <string_view>

namespace engine::ui 
{

    /**
     * UILabel 类用于创建和管理用户界面中的文本标签
     * UILabel 继承自 UIElement，提供了文本渲染功能。
     * 它可以设置文本内容、字体ID、字体大小和文本颜色。
     * 需要一个文本渲染器来获取和更新文本尺寸。
     */
    class UILabel final : public UIElement 
    {
    private:
        engine::render::TextRenderer& text_renderer_;//需要文本渲染器，用于获取/更新文本尺寸

        std::string text_;                          //文本内容    
        std::string font_id_;                       //字体ID
        int font_size_;                             //字体大小   
       SDL_Color text_fcolor_ = { 1, 1, 1, 1 };
    public:
        /**
         * 构造一个UILabel
         * text_renderer 文本渲染器
         * text 文本内容
         * font_id 字体ID
         * font_size 字体大小
         * text_color 文本颜色
         */
        UILabel(engine::render::TextRenderer& text_renderer,
            std::string_view text,
            std::string_view font_id,
            int font_size = 16,
            glm::dvec2 position = { 0.0f, 0.0f },
            SDL_Color text_color = { 255, 255, 255, 255 });

        // --- 核心方法 ---
        void render(engine::core::Context& context) override;

        // --- Setters & Getters ---
        std::string_view get_text() const { return text_; }
        std::string_view get_font_id() const { return font_id_; }
        int get_font_size() const { return font_size_; }
        SDL_Color get_text_color() const { return text_fcolor_; }

        void set_text(std::string_view text);                      //设置文本内容, 同时更新尺寸
        void set_font_id(std::string_view font_id);                 //设置字体ID, 同时更新尺寸
        void set_font_size(int font_size);                         //设置字体大小, 同时更新尺寸
        void set_text_color(SDL_Color text_fcolor);
    };
} // namespace engine::ui