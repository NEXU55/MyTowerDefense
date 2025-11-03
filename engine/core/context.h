#pragma once
#include"../../engine/core/game_state.h"

namespace engine::render 
{
    class Renderer;
    class TextRenderer;
}
namespace engine::resource 
{
    class ResourceManager;
}
namespace engine::audio
{
    class AudioPlayer;
}
namespace engine::core
{
    class Coordinator;
    class ConfigManager;
    class EventBus;
    class InputManager;
}
namespace engine::core
{
	class Context final
	{
        using Renderer = engine::render::Renderer;
        using TextRenderer = engine::render::TextRenderer;
        using ConfigManager = engine::core::ConfigManager;
        using ResourceManager = engine::resource::ResourceManager;
        using Coordinator = engine::core::Coordinator;
        using EventBus = engine::core::EventBus;
        using AudioPlayer = engine::audio::AudioPlayer;
        using InputManager = engine::core::InputManager;

    public:
        bool is_game_win_ = true;
    private:
        // 使用引用，确保每个模块都有效，使用时不需要检查指针是否为空。
        EventBus& eventbus_;                    //事件总线
        ConfigManager& config_;                 //配置文件
        InputManager& input_manager_;           //输入管理器
        ResourceManager& resource_manager_;     //资源管理器
        TextRenderer& text_renderer_;           //文本渲染器
        Renderer& renderer_;                    //渲染器
        AudioPlayer& audio_player_;             //音乐播放器
        Coordinator& coordinator_;              //协调器
        GameState& game_state_;                 //游戏状态
    public:
        Context(EventBus& eventbus,ConfigManager& config,InputManager& input_manager,ResourceManager& resource_manager,
            TextRenderer& text_renderer,Renderer& renderer,AudioPlayer& audio_player ,
            Coordinator& coordinator,GameState& game_state)
            :eventbus_(eventbus)
            ,config_(config)
            ,input_manager_(input_manager)
            ,resource_manager_(resource_manager)
            ,text_renderer_(text_renderer)
            ,renderer_(renderer)
            ,audio_player_(audio_player)
            ,coordinator_(coordinator)
            ,game_state_(game_state)
        { }
        ~Context() = default;
        // 禁止拷贝和移动，Context 对象通常是唯一的或按需创建/传递
        Context(const Context&) = delete;
        Context& operator=(const Context&) = delete;
        Context(Context&&) = delete;
        Context& operator=(Context&&) = delete;

        // --- Getters ---
        TextRenderer& get_text_renderer() const { return text_renderer_; }              //获取文本渲染器
        Renderer& get_renderer() const { return renderer_; }                            //获取渲染器
        ConfigManager& get_config()const { return config_; }                            //获取配置文本
        ResourceManager& get_resource_manager() const { return resource_manager_; }     //获取资源管理器
        Coordinator& get_coordinator()const { return coordinator_; }                    //获取协调器
        EventBus& get_eventbus()const { return eventbus_; }
        AudioPlayer& get_audio_player()const { return audio_player_; }
        InputManager& get_input_manager()const { return input_manager_; }
        GameState get_game_state()const { return game_state_; }
	};
}