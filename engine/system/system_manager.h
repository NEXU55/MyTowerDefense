#pragma once
#include"system.h"
#include<typeindex>
#include<unordered_map>
#include<array>
#include<memory>
#include<spdlog/spdlog.h>

namespace engine::system
{
	class SystemManager
	{
	private:
		std::unordered_map<std::type_index, std::unique_ptr<System>> system_array_;
	public:
		template<class T>
		T* register_system()
		{
			auto type_index = std::type_index(typeid(T));
			auto it = system_array_.find(type_index);
			if (it!=system_array_.end())
			{
				spdlog::trace("已注册该系统:{}", typeid(T).name());
				return static_cast<T*>(it->second.get());
			}
			it=system_array_.emplace(type_index,std::make_unique<T>()).first;
			return static_cast<T*>(it->second.get());
		}

		template<class T>
		void set_signature(Signature signature)
		{
			auto it = system_array_.find(std::type_index(typeid(T)));
			if (it == system_array_.end())
			{
				spdlog::error("不存在该系统:{}", typeid(T).name());
				return;
			}
			it->second->set_signature(signature);
		}

		template<class T>
		Signature get_signature()const
		{
			auto it = system_array_.find(std::type_index(typeid(T)));
			if (it == system_array_.end())
			{
				spdlog::error("不存在该系统:{}", typeid(T).name());
				return Signature();
			}
			return it->second->get_signature();
		}
	};
}