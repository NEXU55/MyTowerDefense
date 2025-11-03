#include "ui_label.h"
#include "../core/context.h"
#include "../render/text_renderer.h"
#include <spdlog/spdlog.h>

namespace engine::ui {

    UILabel::UILabel(engine::render::TextRenderer& text_renderer,
        std::string_view text,
        std::string_view font_id,
        int font_size,glm::dvec2 position,SDL_Color text_color)
        : UIElement(std::move(position)),
        text_renderer_(text_renderer),
        text_(text),
        font_id_(font_id),
        font_size_(font_size),
        text_fcolor_(std::move(text_color)) 
    {
        // 获取文本渲染尺寸
        size_ = text_renderer_.get_text_size(text_, font_id_, font_size_);
        spdlog::trace("UILabel 构造完成");
    }

    void UILabel::render(engine::core::Context& context) 
    {
        if (!visible_ || text_.empty()) 
            return;
        text_renderer_.draw_ui_text(text_, font_id_, font_size_, get_screen_position(), text_fcolor_);
        // 渲染子元素（调用基类方法）
        UIElement::render(context);
    }

    void UILabel::set_text(std::string_view text)
    {
        text_ = text;
        size_ = text_renderer_.get_text_size(text_, font_id_, font_size_);
    }

    void UILabel::set_font_id(std::string_view font_id)
    {
        font_id_ = font_id;
        size_ = text_renderer_.get_text_size(text_, font_id_, font_size_);
    }

    void UILabel::set_font_size(int font_size)
    {
        font_size_ = font_size;
        size_ = text_renderer_.get_text_size(text_, font_id_, font_size_);
    }

    void UILabel::set_text_color(SDL_Color text_fcolor)
    {
        text_fcolor_ = std::move(text_fcolor);
        /* 颜色变化不影响尺寸 */
    }
}