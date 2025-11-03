#include"scene.h"
#include"../core/context.h"
#include"../../engine/ui/ui_manager.h"
#include"../../engine/core/coordinator.h"

#include"../../engine/system/physics_system.h"
#include"../../engine/system/collision_system.h"

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
		physics_system_ = context_.get_coordinator().register_system<PhysicsSystem>();
		collision_system_ = context_.get_coordinator().register_system<CollisionSystem>();
	}
	void Scene::input(const InputEvent& event)
	{
		ui_manager_->input(context_,event);
	}
	void Scene::update(double delta)
	{
		if (context_.get_game_state() == engine::core::GameState::Playing)
		{
			physics_system_->update(delta, context_);
			collision_system_->update(delta, context_);
		}
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