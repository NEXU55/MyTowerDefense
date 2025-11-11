#include "ui_interactive.h"
#include "ui_state.h"
#include "../core/context.h"
#include "../render/renderer.h"
#include "../resource/resource_manager.h"
#include "../audio/audio_player.h"
#include <spdlog/spdlog.h>

namespace engine::ui 
{

    UIInteractive::~UIInteractive() = default;

    UIInteractive::UIInteractive(Context& context, glm::dvec2 position, glm::dvec2 size)
        : UIElement(std::move(position), std::move(size)), context_(context)
    {
        spdlog::trace("UIInteractive 构造完成");
    }

    void UIInteractive::set_state(std::unique_ptr<engine::ui::UIState> state)
    {
        if (!state) 
        {
            spdlog::warn("尝试设置空的状态！");
            return;
        }
        if (state_)
            state_->exit();
        state_ = std::move(state);
        state_->enter();
    }

    void UIInteractive::add_texture(std::string_view name, std::string_view path)
    {
        auto& resource_manager = context_.get_resource_manager();
        auto texture = resource_manager.get_texture(path);

        // 可交互UI元素必须有一个size用于交互检测，因此如果参数列表中没有指定，则用图片大小作为size
        if (size_.x == 0.0 || size_.y == 0.0) 
            size_ = resource_manager.get_texture_size(texture);
        // 添加精灵
        textures_[std::string(name)] = std::move(texture);
    }

    void UIInteractive::set_texture(std::string_view name)
    {
        if (textures_.find(std::string(name)) != textures_.end())
            current_texture_ = textures_[std::string(name)];
        else 
            spdlog::warn("Texture :'{}' 未找到", name);
    }

    void UIInteractive::add_sound(std::string_view name, std::string_view path)
    {
        //sounds_.emplace(std::string(name),std::string(path));
    }

    void UIInteractive::play_sound(std::string_view name)
    {
       /* auto it = sounds_.find(std::string(name));
        if (it != sounds_.end())
            context_.get_audio_player().play_sound(it->second);
        else
            spdlog::error("Sound '{}' 未找到", name);*/
    }

    void UIInteractive::update(double delta, Context& context)
    {
        // 先更新子节点，再更新自己（状态）
        if (state_ && interactive_)
        {
            auto ptr=state_->update(context);
            if(ptr)
                set_state(std::move(ptr));
        }
    }

    bool UIInteractive::input(Context& context, const InputEvent& event)
    {
        if (UIElement::input(context,event)) 
            return true;
        // 先更新子节点，再更新自己（状态）
        if (state_ && interactive_) 
        {
            if (auto next_state = state_->input(context,event); next_state) 
            {
                set_state(std::move(next_state));
                return true;
            }
        }
        return false;
    }

    void UIInteractive::render(Context& context)
    {
        if (!visible_) 
            return;
        // 先渲染自身
        SDL_Rect src_rect{0,0,size_.x,size_.y};
        context.get_renderer().draw_sprite(current_texture_,src_rect,get_screen_position(),0);

        // 再渲染子元素（调用基类方法）
        UIElement::render(context);
    }
}