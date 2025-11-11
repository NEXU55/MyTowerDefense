#pragma once
#include"../core/type.h"
#include"../collision/collision_mask.h"

#include<variant>
#include<glm/vec2.hpp>

namespace engine::component
{
    struct CollisionComponent
    {
		using CollisionBoxType=engine::collision::CollisionBoxType;
		struct Rect
		{
			double left=0, right=0, up=0, down=0;
		};

		bool enabled = true;							//是否生效
		int collision_mask = 0;							//碰撞掩码
		int layer = 0;									//碰撞层级
		CollisionBoxType type = CollisionBoxType::None; //碰撞盒形状
		std::variant<double, glm::dvec2> size;			//大小（半径/宽高）
		Entity owner = -1;								//持有者
		Signature event_type;							//触发事件类型
		Rect rect;
    };
}