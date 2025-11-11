#pragma once
#include"../core/type.h"
#include<unordered_set>
#include<memory>

namespace engine::core
{
	class Context;
}

namespace engine::system
{
	class System
	{
	protected:
		using Context = engine::core::Context;
	public:
		virtual ~System() = default;
		virtual void init(Context&) = 0;
		virtual void update(double, Context&){}

		Signature get_signature()const { return signature_; }
		void set_signature(Signature signature) { signature_ = signature; }
	protected:
		Signature signature_;
		std::vector<std::shared_ptr<void>> events_;
	};
}