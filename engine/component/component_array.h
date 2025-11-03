#pragma once
#include"../core/type.h"
#include<vector>
#include<spdlog/spdlog.h>

namespace engine::component
{
	class BaseComponentArray
	{
	public:
		BaseComponentArray() = default;
		virtual ~BaseComponentArray() = default;
		virtual void destroy_entity(Entity) = 0;
		virtual void clear(){}
	};

	template<class T>
	class ComponentArray final :public BaseComponentArray
	{
	private:
		T fault_;
		size_t size_ = 0;
		std::uint32_t capacity_ = 0;
		std::vector<Index> index_array_;//稀疏数组，索引为entity,存储组件的index
		std::vector<T> component_array_;//密集数组，存储组件
		std::vector<Entity> entity_array_;//用于删除时更新稀疏数组
	public:
		ComponentArray(std::uint32_t capacity)
			:capacity_(capacity)
		{
			index_array_.resize(MAX_ENTITY, -1);
			component_array_.resize(capacity);
			entity_array_.resize(capacity, -1);
		}

		void insert(Entity entity, T component)
		{
			if (size_ >= capacity_)
			{
				spdlog::warn("组件列表已满，无法插入组件");
				return;
			}
			Index index = size_;
			index_array_[entity] = index;
			entity_array_[index] = entity;
			component_array_[index] = component;
			++size_;
		}

		void remove(Entity entity)
		{
			Index index = index_array_[entity];
			if (index == -1)
			{
				spdlog::error("不存在要删除的实体，ID：{}", entity);
				return;
			}
			Index last_index = size_ - 1;
			Entity last_entity = entity_array_[last_index];
			if (index != last_index)
			{
				//交换
				index_array_[last_entity] = index;//
				component_array_[index] = component_array_[last_index];
				entity_array_[index] = last_entity;//
			}
			//复原	
			index_array_[entity] = -1;
			entity_array_[last_index] = -1;
			component_array_[last_index] = T();
			--size_;
		}

		T& get(Entity entity)
		{
			Index index = index_array_[entity];
			if (index == -1)
			{
				spdlog::warn("使用了不存在的实体,ID:{}", entity);
				return fault_;
			}
			return component_array_[index];
		}

		std::pair<Entity, T&> get_component_singleton()
		{
			if (capacity_ != 1)
			{
				spdlog::error("不是单例组件,{},实际容量为:{}", typeid(T).name(), capacity_);
				return { -1,fault_ };
			}
			return { entity_array_[0],component_array_[0] };
		}

		void clear()override
		{
			component_array_ = std::vector<T>(capacity_);
			index_array_ = std::vector<Index>(MAX_ENTITY, -1);
			entity_array_ = std::vector<Entity>(capacity_, -1);
			size_ = 0;
		}

		void destroy_entity(Entity entity)override
		{
			if (index_array_[entity] != -1)
				remove(entity);
		}

		size_t get_size()const
		{
			return size_;
		}
	};
}
