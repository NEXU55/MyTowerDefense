#pragma once
#include "../core/context.h"
#include"../core/event_bus.h"
#include "scene.h"
#include <spdlog/spdlog.h>
#include <memory>
#include <string>
#include <vector>

namespace engine::scene 
{
     //管理游戏中的场景栈，处理场景切换和生命周期。
    class SceneManager final 
    {
        using Context = engine::core::Context;
        enum class PendingAction                                //待处理的动作
        { 
            None, 
            Push, 
            Pop, 
            Replace 
        };  
    private:
        Context& context_;                                      //引擎上下文引用
        std::vector<std::unique_ptr<Scene>> scene_stack_;        //场景栈
        PendingAction pending_action_ = PendingAction::None;    //待处理的动作
        std::unique_ptr<Scene> pending_scene_;                  //待处理场景
        std::shared_ptr<void> input_event_ptr_;
    public:
        explicit SceneManager(Context& context)
            : context_(context) 
        {
            input_event_ptr_=context.get_eventbus().subscribe<engine::event::InputEvent>(
                [this](const engine::event::InputEvent& event)
                {
                    input(event);
                });

            spdlog::trace("场景管理器已创建。");
        }
        ~SceneManager()
        {
            spdlog::trace("场景管理器已销毁。");
            close(); // 即使不手动调用 close 也能确保清理
        }

        // 禁止拷贝和移动
        SceneManager(const SceneManager&) = delete;
        SceneManager& operator=(const SceneManager&) = delete;
        SceneManager(SceneManager&&) = delete;
        SceneManager& operator=(SceneManager&&) = delete;

        // 延时切换场景
        void request_push(std::unique_ptr<Scene>&& scene)      //请求压入一个新场景。
        {
            pending_action_ = PendingAction::Push;
            pending_scene_ = std::move(scene);
        }
        void request_pop()                                     //请求弹出当前场景。
        {
            pending_action_ = PendingAction::Pop;
        }
        void request_replace(std::unique_ptr<Scene>&& scene)   //请求替换当前场景。
        {
            pending_action_ = PendingAction::Replace;
            pending_scene_ = std::move(scene);
        }

        // getters
        Scene* get_current_scene() const//获取当前活动场景（栈顶场景）的指针
        {
            if (scene_stack_.empty()) 
                return nullptr;
            return scene_stack_.back().get(); // 返回栈顶场景的裸指针
        }
        Context& get_context() const { return context_; }            //获取引擎上下文引用。

        // 核心循环函数
        void update(double delta)
        {
            // 只更新栈顶（当前）场景
            Scene* current_scene = get_current_scene();
            if (current_scene) 
            {
                current_scene->update(delta);
            }
            // 执行可能的切换场景操作
            process_pending_action();
        }
        void render()
        {
            // 渲染时需要叠加渲染所有场景，而不只是栈顶
            for (const auto& scene : scene_stack_) 
            {
                if (scene)
                    scene->render();
            }
        }
        void input(const engine::event::InputEvent& event)
        {
            // 只考虑栈顶场景
            Scene* current_scene = get_current_scene();
            if (current_scene)
                current_scene->input(event);
        }

        void close()
        {
            spdlog::trace("正在关闭场景管理器并清理场景栈...");
            // 清理栈中所有剩余的场景（从顶到底）
            while (!scene_stack_.empty()) 
            {
                if (scene_stack_.back()) 
                {
                    spdlog::debug("正在清理场景 '{}' 。", scene_stack_.back()->get_name());
                    scene_stack_.back()->clear();
                }
                scene_stack_.pop_back();
            }
        }

    private:
        void process_pending_action()                           ///< @brief 处理挂起的场景操作（每轮更新最后调用）。
        {
            if (pending_action_ == PendingAction::None)
                return;
            switch (pending_action_) 
            {
            case PendingAction::Pop:
                pop();
                break;
            case PendingAction::Replace:
                replace(std::move(pending_scene_));
                break;
            case PendingAction::Push:
                push(std::move(pending_scene_));
                break;
            default:
                break;
            }

            pending_action_ = PendingAction::None;
        }
        // 直接切换场景
        void push(std::unique_ptr<Scene>&& scene)         ///< @brief 将一个新场景压入栈顶，使其成为活动场景。
        {
            if (!scene) 
            {
                spdlog::warn("尝试将空场景压入栈。");
                return;
            }
            spdlog::debug("正在将场景 '{}' 压入栈。", scene->get_name());
            // 初始化新场景
            if (!scene->is_initialized())// 确保只初始化一次  
                scene->init();
            // 将新场景移入栈顶
            scene_stack_.push_back(std::move(scene));
        }

        void pop()                                        ///< @brief 移除栈顶场景。
        {
            if (scene_stack_.empty()) 
            {
                spdlog::warn("尝试从空场景栈中弹出。");
                return;
            }
            spdlog::debug("正在从栈中弹出场景 '{}' 。", scene_stack_.back()->get_name());
            // 清理并移除栈顶场景
            if (scene_stack_.back())
                scene_stack_.back()->clear();
            scene_stack_.pop_back();
        }
        void replace(std::unique_ptr<Scene>&& scene)      ///< @brief 清理场景栈所有场景，将此场景设为栈顶场景。
        {
            if (!scene) 
            {
                spdlog::warn("尝试用空场景替换。");
                return;
            }
            spdlog::debug("正在用场景 '{}' 替换场景 '{}' 。", scene->get_name(), scene_stack_.back()->get_name());
            // 清理并移除场景栈中所有场景
            while (!scene_stack_.empty()) 
            {
                if (scene_stack_.back())
                    scene_stack_.back()->clear();
                scene_stack_.pop_back();
            }
            // 初始化新场景
            if (!scene->is_initialized())
                scene->init();
            // 将新场景压入栈顶
            scene_stack_.push_back(std::move(scene));
        }
    };
} // namespace engine::scene