#pragma once
#include "ui_element.h"
#include "ui_state.h"
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

typedef struct SDL_Texture SDL_Texture;
namespace engine::core 
{
    class Context;
}

namespace engine::ui 
{
    /*
     * 可交互UI元素的基类,继承自UIElement
     * 定义了可交互UI元素的通用属性和行为。
     * 管理UI状态的切换和交互逻辑。
     * 提供事件处理、更新和渲染的虚方法。
     */
    class UIInteractive : public UIElement 
    {
    protected:
        engine::core::Context& context_;                        //可交互元素很可能需要其他引擎组件
        std::unique_ptr<engine::ui::UIState> state_;            //当前状态
        std::unordered_map<std::string, SDL_Texture*> textures_;//纹理集合
        //std::unordered_map<std::string, std::string> sounds_;   //音效集合，key为音效名称，value为音效文件路径
        SDL_Texture* current_texture_ = nullptr;                //当前显示的纹理
        bool interactive_ = true;                               //是否可交互

    public:
        UIInteractive(Context& context, glm::dvec2 position = { 0.0, 0.0 }, glm::dvec2 size = { 0.0, 0.0 });
        ~UIInteractive() override;

        virtual void clicked() {}       //如果有点击事件，则重写该方法
        virtual void hovered() {}       //如果有悬停事件，则重写该方法
        virtual void unhovered(){}      //如果有取消悬停事件，则重写该方法

        void add_texture(std::string_view name, std::string_view path);//添加纹理
        void set_texture(std::string_view name);                       //设置当前显示的纹理
        void add_sound(std::string_view name, std::string_view path);  //添加音效
        void play_sound(std::string_view name);                        //播放音效
        // --- Getters and Setters ---
        void set_state(std::unique_ptr<engine::ui::UIState> state);    //设置当前状态
        engine::ui::UIState* get_state() const { return state_.get(); }//获取当前状态

        void setInteractive(bool interactive) { interactive_ = interactive; }   //设置是否可交互
        bool isInteractive() const { return interactive_; }                     //获取是否可交互

        // --- 核心方法 ---
        void update(double delta, Context& context)override;
        bool input(Context& context, const InputEvent& event) override;
        void render(Context& context) override;
    };

} // namespace engine::ui