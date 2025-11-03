#pragma once
#include<nlohmann/json_fwd.hpp>
#include<unordered_map>
#include<unordered_set>

namespace engine::resource
{
	class AudioSetManager
	{
		using json = nlohmann::json;
	private:
		std::unordered_map<std::string, std::unordered_map<std::string,std::vector<std::string>>> audio_set_pool_;
	public:
		void load(const std::string&);

		std::string_view get(std::string_view type, std::string_view name)
		{
			auto& pool = audio_set_pool_[type.data()][name.data()];
			int index = rand() % pool.size();
			return pool[index];
		}

		void clear()
		{
			audio_set_pool_.clear();
		}
	};
}