#include"input_manager.h"
#include"../event/input_event.h"
#include"event_bus.h"
#include<spdlog/spdlog.h>

namespace engine::core
{
    InputManager::InputManager(EventBus& event_bus,const std::unordered_map<std::string, std::vector<std::string>>& input_list)
        :event_bus_(event_bus)
    {
        initializeMappings(input_list);
        is_running_ = true;
    }

    // --- 更新和事件处理 ---

    void InputManager::update() 
    {
        //处理所有待处理的 SDL 事件 
        while (SDL_PollEvent(&event_)) 
        {
            process_event();
        }

        //发送一个无任何动作的事件，用于激活由鼠标移动驱动的事件
        event_bus_.publish(engine::event::InputEvent());

        //根据上一帧的值更新默认的动作状态
        for (auto& [action_name, state] : action_states_) 
        {
            if (state == ActionState::INACTIVE)
                continue;
            engine::event::InputEvent input_event;
            input_event.state = state;
            input_event.message = action_name;

            event_bus_.publish(input_event);
            if (state == ActionState::PRESSED_THIS_FRAME) 
                state = ActionState::HELD_DOWN;
            else if (state == ActionState::RELEASED_THIS_FRAME) 
                state = ActionState::INACTIVE;
        }
    }

    void InputManager::process_event() 
    {
        switch (event_.type) 
        {
        case SDL_KEYDOWN:
        case SDL_KEYUP: 
        {
            SDL_Scancode scancode = event_.key.keysym.scancode;     // 获取按键的scancode
            bool is_down = event_.key.state==SDL_PRESSED;
            bool is_repeat = event_.key.repeat;

            auto it = input_to_actions_map_.find(scancode);
            if (it != input_to_actions_map_.end()) 
            {     // 如果按键有对应的action
                const std::vector<std::string>& associated_actions = it->second;
                for (const auto& action_name : associated_actions) 
                {
                    update_action_state(action_name, is_down, is_repeat); // 更新action状态
                }
            }
            break;
        }
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP: 
        {
            Uint32 button = event_.button.button;              // 获取鼠标按钮
            bool is_down = event_.button.state==SDL_PRESSED;
            auto it = input_to_actions_map_.find(button);
            if (it != input_to_actions_map_.end()) 
            {     // 如果鼠标按钮有对应的action
                const std::vector<std::string>& associated_actions = it->second;
                for (const auto& action_name : associated_actions) 
                {
                    // 鼠标事件不考虑repeat, 所以第三个参数传false
                    update_action_state(action_name, is_down, false); // 更新action状态
                }
            }
            break;
        }
        case SDL_QUIT:
            is_running_ = false;
            break;
        }
    }

    // --- 初始化输入映射 ---

    void InputManager::initializeMappings(const std::unordered_map<std::string, std::vector<std::string>>& input_list) {
        spdlog::trace("初始化输入映射...");
        actions_to_keyname_map_ = input_list;      // 获取配置中的输入映射（动作 -> 按键名称）
        input_to_actions_map_.clear();
        action_states_.clear();

        // 如果配置中没有定义鼠标按钮动作(通常不需要配置),则添加默认映射, 用于 UI
        if (actions_to_keyname_map_.find("MouseLeftClick") == actions_to_keyname_map_.end()) 
        {
            spdlog::debug("配置中没有定义 'MouseLeftClick' 动作,添加默认映射到 'MouseLeft'.");
            actions_to_keyname_map_["MouseLeftClick"] = { "MouseLeft" };     // 如果缺失则添加默认映射
        }
        if (actions_to_keyname_map_.find("MouseRightClick") == actions_to_keyname_map_.end()) 
        {
            spdlog::debug("配置中没有定义 'MouseRightClick' 动作,添加默认映射到 'MouseRight'.");
            actions_to_keyname_map_["MouseRightClick"] = { "MouseRight" };   // 如果缺失则添加默认映射
        }
        // 遍历 动作 -> 按键名称 的映射
        for (const auto& [action_name, key_names] : actions_to_keyname_map_) 
        {
            // 每个动作对应一个动作状态，初始化为 INACTIVE
            action_states_[action_name] = ActionState::INACTIVE;
            spdlog::trace("映射动作: {}", action_name);
            // 设置 "按键 -> 动作" 的映射
            for (const auto& key_name : key_names) {
                SDL_Scancode scancode = scancodeFromString(key_name);       // 尝试根据按键名称获取scancode
                Uint32 mouse_button = mouseButtonFromString(key_name);  // 尝试根据按键名称获取鼠标按钮
                // 未来可添加其它输入类型 ...

                if (scancode != SDL_SCANCODE_UNKNOWN) 
                {      
                    // 如果scancode有效,则将action添加到scancode_to_actions_map_中
                    input_to_actions_map_[scancode].emplace_back(action_name);
                    spdlog::trace("  映射按键: {} (Scancode: {}) 到动作: {}", key_name, static_cast<int>(scancode), action_name);
                }
                else if (mouse_button != 0) 
                {             
                    // 如果鼠标按钮有效,则将action添加到mouse_button_to_actions_map_中
                    input_to_actions_map_[mouse_button].emplace_back(action_name);
                    spdlog::trace("  映射鼠标按钮: {} (Button ID: {}) 到动作: {}", key_name, static_cast<int>(mouse_button), action_name);
                    // else if: 未来可添加其它输入类型 ...
                }
                else
                    spdlog::warn("输入映射警告: 未知键或按钮名称 '{}' 用于动作 '{}'.", key_name, action_name);
            }
        }
        spdlog::trace("输入映射初始化完成.");
    }

    // --- 工具函数 ---
    // 将字符串名称转换为 SDL_Scancode
    SDL_Scancode InputManager::scancodeFromString(std::string_view key_name) 
    {
        return SDL_GetScancodeFromName(key_name.data());
    }

    // 将鼠标按钮名称字符串转换为 SDL 按钮 Uint32 值
    Uint32 InputManager::mouseButtonFromString(std::string_view button_name) 
    {
        if (button_name == "MouseLeft") return SDL_BUTTON_LEFT;
        if (button_name == "MouseMiddle") return SDL_BUTTON_MIDDLE;
        if (button_name == "MouseRight") return SDL_BUTTON_RIGHT;
        // SDL 还定义了 SDL_BUTTON_X1 和 SDL_BUTTON_X2
        if (button_name == "MouseX1") return SDL_BUTTON_X1;
        if (button_name == "MouseX2") return SDL_BUTTON_X2;
        return 0; // 0 不是有效的按钮值，表示无效
    }

    void InputManager::update_action_state(std::string_view action_name, bool is_input_active, bool is_repeat_event) 
    {
        auto it = action_states_.find(std::string(action_name));
        if (it == action_states_.end()) 
        {
            spdlog::warn("尝试更新未注册的动作状态: {}", action_name);
            return;
        }
        if (is_input_active) 
        { 
            // 输入被激活 (按下)
            if (is_repeat_event) 
                it->second = ActionState::HELD_DOWN;
            else 
                it->second = ActionState::PRESSED_THIS_FRAME;
        }
        else 
        { 
            // 输入被释放 (松开)
            it->second = ActionState::RELEASED_THIS_FRAME;
        }
    }
}