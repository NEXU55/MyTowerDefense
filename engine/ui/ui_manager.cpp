#include"ui_manager.h"
#include"ui_panel.h"
#include"ui_element.h"
#include <spdlog/spdlog.h>

namespace engine::ui 
{
    UIManager::~UIManager() = default;
    UIManager::UIManager() 
    {
        // 创建一个无特定大小和位置的Panel，它的子元素将基于它定位。
        root_element_ = std::make_unique<UIPanel>(glm::dvec2{ 0.0, 0.0 }, glm::dvec2{ 0.0, 0.0 });
        spdlog::trace("UI管理器构造完成。");
    }

    bool UIManager::init(const glm::dvec2& window_size) 
    {
        root_element_->set_size(window_size);
        spdlog::trace("UI管理器已初始化根面板。");
        return true;
    }

    void UIManager::add_element(std::unique_ptr<UIElement> element) 
    {
        if (root_element_)
            root_element_->add_child(std::move(element));
        else
            spdlog::error("无法添加元素：root_element_ 为空！");
    }

    void UIManager::clear() 
    {
        if (root_element_) 
        {
            root_element_->remove_all_children();
            spdlog::trace("所有UI元素已从UI管理器中清除。");
        }
    }

    bool UIManager::input(Context& context, const InputEvent& event)
    {
        if (root_element_ && root_element_->is_visible()) 
        {
            // 从根元素开始向下分发事件
            if (root_element_->input(context,event)) return true;
        }
        return false;
    }

    void UIManager::update(double delta, Context& context) {
        if (root_element_ && root_element_->is_visible())
            // 从根元素开始向下更新
            root_element_->update(delta, context);
    }

    void UIManager::render(Context& context)
    {
        if (root_element_ && root_element_->is_visible()) 
        {
            // 从根元素开始向下渲染
            root_element_->render(context);
        }
    }

    UIPanel* UIManager::get_root_element() const 
    {
        return root_element_.get();
    }
}