#pragma once
#include"component_array.h"
#include<typeindex>//用于std::type_index
#include<memory>
#include<functional>
#include<spdlog/spdlog.h>

namespace engine::component
{
	class ComponentManager
	{
	private:
		std::unordered_map<std::type_index, ComponentType> component_type_list_;
		std::array<std::unique_ptr<BaseComponentArray>, MAX_COMPONENT> components_list_;
		ComponentType next_component_type_ = 0;
	public:
		template<class T>
		void register_component(std::uint32_t capacity)
		{
			auto type_index = std::type_index(typeid(T));
			if (component_type_list_.count(type_index)!=0)
			{
				spdlog::error("已注册该类组件,{}", typeid(T).name());
				return;
			}
			components_list_[next_component_type_]=std::make_unique<ComponentArray<T>>(capacity);
			component_type_list_.emplace(type_index,next_component_type_);
			++next_component_type_;
		}

		template<class T>
		void add_component(Entity entity, T component)
		{
			get_component_array<T>()->insert(entity, component);
		}

		template<class T>
		void remove_component(Entity entity)
		{
			get_component_array<T>()->remove(entity);
		}

		template<class T>
		ComponentType get_component_type()
		{
			auto type_index = std::type_index(typeid(T));
			auto it = component_type_list_.find(type_index);
			if (it==component_type_list_.end())
			{
				spdlog::warn("不存在该类组件,{}", typeid(T).name());
				return -1;
			}
			return it->second;
		}

		template<class T>
		T& get_component(Entity entity)
		{
			return get_component_array<T>()->get(entity);
		}

		template<class T>
		std::pair<Entity,T&> get_component_singleton()
		{
			return get_component_array<T>()->get_component_singleton();
		}

		void destroy_entity(Entity entity,Signature signautre)
		{
			for (int i = 0; i < MAX_COMPONENT; ++i)
			{
				if (signautre[i] == 1)
				{
					components_list_[i]->destroy_entity(entity);
				}
			}
		}

		template<class T>
		size_t get_component_size()
		{
			return get_component_array<T>()->get_size();
		}

		void clear()
		{
			for (int i=0;i< component_type_list_.size();++i)
			{
				components_list_[i]->clear();
			}
		}
	private:
		template<class T>
		ComponentArray<T>* get_component_array()
		{
			auto it = component_type_list_.find(std::type_index(typeid(T)));
			if (it == component_type_list_.end())
			{
				spdlog::error("尝试获取不存在的组件数组:{}", typeid(T).name());
				return nullptr;
			}
			ComponentType type = it->second;
			return static_cast<ComponentArray<T>*>(components_list_[type].get());
		}
	};
}