#pragma once
#include"../core/type.h"
#include<queue>
#include<array>
#include<spdlog/spdlog.h>

namespace engine::entity
{
	class EntityManager
	{
	private:
		std::queue<Entity> available_entity_list_;//未使用的实体ID列表

		std::array<Index, MAX_ENTITY> index_array_;//稀疏数组，索引为实体ID，存储密集数组的索引
		std::array<Signature, MAX_ENTITY> entity_array_;//密集数组，现存的实体列表，存储的是对应的签名
		std::array<Entity, MAX_ENTITY> version_array_;//版本号数组，用于删除时更新
		uint32_t size_ = 0;
	public:
		EntityManager()
		{
			index_array_.fill(-1);
			entity_array_.fill(Signature());
			version_array_.fill(-1);
			for (Entity entity = 0; entity < MAX_ENTITY; ++entity)
			{
				available_entity_list_.emplace(entity);
			}
		}

		bool is_entity_valid(Entity entity)const
		{
			return entity < MAX_ENTITY&&index_array_[entity]!=-1;
		}

		Entity create_entity()
		{
			if (size_ >= MAX_ENTITY)
			{
				spdlog::warn("实体列表已满，无法创建实体");
				return -1;
			}
			Entity id = available_entity_list_.front();
			available_entity_list_.pop();

			index_array_[id] = size_;
			version_array_[size_] =id;

			++size_;
			return id;
		}

		void destroy_entity(Entity entity)
		{
			Index index = index_array_[entity];
			if (index == -1)
			{
				spdlog::error("不存在要删除的实体，ID：{}", entity);
				return;
			}
			Index last_index = size_ - 1;
			Entity last_entity = version_array_[last_index];
			if (index != last_index)
			{
				//交换
				index_array_[last_entity] = index;
				entity_array_[index] = entity_array_[last_index];
				version_array_[index] = last_entity;
			}
			//复原
			index_array_[entity] = -1;
			entity_array_[last_index].reset();
			version_array_[last_index] = -1;
			--size_;

			available_entity_list_.push(entity);
		}

		void set_signautre(Entity entity, Signature signature)
		{
			Index index = index_array_[entity];
			if (index == -1)
			{
				spdlog::error("无法设置不存在的实体签名，ID：{}", entity);
				return;
			}
			entity_array_[index]=signature;
		}

		Signature get_signature(Entity entity) 
		{
			if(entity==-1)
				return Signature();
			Index index = index_array_[entity];
			if (index == -1)
			{
				spdlog::error("无法获取不存在的实体签名，ID：{}", entity);
				return Signature();
			}
			return entity_array_[index];
		}

		void clear()
		{
			for (Index index = 0; index < size_; ++index)
			{
				available_entity_list_.emplace(version_array_[index]);
			}
			index_array_.fill(-1);
			entity_array_.fill(Signature());
			version_array_.fill(-1);
			size_=0;
		}
	};
}