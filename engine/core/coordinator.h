#pragma once
#include"../component/component_manager.h"
#include"../system/system_manager.h"
#include"../entity/entity_manager.h"

namespace engine::core
{
	class Coordinator
	{
		using ComponentManager = engine::component::ComponentManager;
		using EntityManager = engine::entity::EntityManager;
		using SystemManager = engine::system::SystemManager;
	private:
		ComponentManager component_manager_;
		EntityManager entity_manager_;
		SystemManager system_manager_;
	public:
		//ALL
		void clear()
		{
			component_manager_.clear();
			entity_manager_.clear();
			system_manager_.clear();
		}

		//Entity
		Entity create_entity()
		{
			return entity_manager_.create_entity();
		}

		void destory_entity(Entity entity)
		{
			component_manager_.destroy_entity(entity,entity_manager_.get_signature(entity));
			system_manager_.destroy_entity(entity);
			entity_manager_.destroy_entity(entity);
			spdlog::trace("删除实体，ID：{}", entity);
		}

		bool is_entity_valid(Entity entity)const
		{
			return entity_manager_.is_entity_valid(entity);
		}
		//Component
		template<class T>
		void register_component(std::uint32_t capacity=MAX_ENTITY)
		{
			component_manager_.register_component<T>(capacity);
		}

		template<class T>
		void register_component_singleton()
		{
			component_manager_.register_component<T>(1);
		}

		template<class T>
		void add_component(Entity entity,T component)
		{
			auto signature = entity_manager_.get_signature(entity);
			signature.set(component_manager_.get_component_type<T>(), true);
			component_manager_.add_component(entity, component);
			entity_manager_.set_signautre(entity,signature);
			system_manager_.insert_entity(entity,signature);
		}

		template<class T>
		void remove_component(Entity entity)
		{
			auto signature = entity_manager_.get_signature(entity);
			signature.set(component_manager_.get_component_type<T>(), false);
			component_manager_.remove_component<T>(entity);
			entity_manager_.set_signautre(signature);
			system_manager_.erase_entity(entity, signature);
		}

		template<class T>
		T& get_component(Entity entity)
		{
			return component_manager_.get_component<T>(entity);
		}

		template<class T>
		std::pair<Entity, T&> get_component_singleton()
		{
			return component_manager_.get_component_singleton<T>();
		}

		template<class T>
		ComponentType get_component_type()
		{
			return component_manager_.get_component_type<T>();
		}

		template<class T>
		size_t get_component_size()
		{
			return component_manager_.get_component_size<T>();
		}

		template<class T>
		bool has_component(Entity entity)
		{
			return is_entity_valid(entity) && entity_manager_.get_signature(entity)[get_component_type<T>()];
		}
		//System
		template<class T>
		T* register_system()
		{
			T* ptr = system_manager_.register_system<T>();
			return ptr;
		}

		template<class T>
		void set_signature(Signature signature)
		{
			system_manager_.set_signature<T>(signature);
		}

		template<class T>
		Signature get_signature()const
		{
			return system_manager_.get_signature<T>();
		}
	};
}