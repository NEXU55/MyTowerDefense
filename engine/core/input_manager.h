#pragma once
#include"type.h"
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <variant>
#include<glm/vec2.hpp>
#include <SDL.h>

namespace engine::core 
{
    class EventBus;
    class InputManager final 
    {
    private:
        std::unordered_map<std::string, std::vector<std::string>> actions_to_keyname_map_;                      //存储动作名称到按键名称列表的映射
        std::unordered_map<std::variant<SDL_Scancode, Uint32>, std::vector<std::string>> input_to_actions_map_; //从输入到关联的动作名称列表
        std::unordered_map<std::string, ActionState> action_states_;                                            //存储每个动作的当前状态

        bool is_running_ = false;
        SDL_Event event_;
        EventBus& event_bus_;
    public:
        InputManager(EventBus& event_bus,const std::unordered_map<std::string, std::vector<std::string>>& input_list);

        void update();//更新输入状态，每轮循环最先调用

        glm::dvec2 get_mouse_position()const
        {
            int mouseX=0, mouseY=0;
            Uint32 buttonState = SDL_GetMouseState(&mouseX, &mouseY);
            return { mouseX,mouseY };
        }

        bool get_is_running()const { return is_running_; }
    private:
        void process_event();//处理 SDL 事件（将按键转换为动作状态）
        void initializeMappings(const std::unordered_map<std::string, std::vector<std::string>>& input_list);//根据 Config配置初始化映射表

        void update_action_state(std::string_view action_name, bool is_input_active, bool is_repeat_event); //辅助更新动作状态
        SDL_Scancode scancodeFromString(std::string_view key_name);                                       //将字符串键名转换为 SDL_Scancode
        Uint32 mouseButtonFromString(std::string_view button_name);                                       //将字符串按钮名转换为 SDL_Button
    };
}