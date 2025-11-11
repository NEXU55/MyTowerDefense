#include"scene.h"
#include"../core/context.h"
#include"../../engine/ui/ui_manager.h"
#include"../../engine/core/coordinator.h"

namespace engine::scene
{
	Scene::Scene(Context& context, std::string_view name)
		:context_(context)
		,ui_manager_(std::make_unique<engine::ui::UIManager>())
		,name_(name)
	{}
	Scene::~Scene() = default;
	void Scene::init()
	{ 
		
	}
	void Scene::input(const InputEvent& event)
	{
		ui_manager_->input(context_,event);
	}
	void Scene::update(double delta)
	{
		ui_manager_->update(delta, context_);
	}
	void Scene::render()
	{
		ui_manager_->render(context_);
	}

	void Scene::clear()
	{ 
		is_initialized_ = false; 
	}
}