#pragma once
#include"system.h"
#include"../core/event_bus.h"

#include"../collision/collision_mask.h"
#include"../event/collision_event.h"
#include<glm/vec2.hpp>

namespace engine::component
{
	struct TransformComponent;
	struct CollisionComponent;
}

namespace engine::system
{
	//负责更新动画帧序列，确定精灵组件的源矩形
	class CollisionSystem final :public System
	{
		using  TransformComponent = engine::component::TransformComponent;
		using  CollisionComponent = engine::component::CollisionComponent;
		using  CollisionBoxType = engine::collision::CollisionBoxType;
		using  CollisionEvent = engine::event::CollisionEvent;
	private:
		bool model = true;
	public:
		void init(Context& context)override;
		void update(double, Context&)override;
	private:
		bool is_collide(const CollisionComponent& a, const CollisionComponent& b)const;

		bool check_rec_rec(const CollisionComponent& a, const CollisionComponent& b)const;
		bool check_cir_cir(const CollisionComponent& a, const CollisionComponent& b)const;
		bool check_rec_cir(const CollisionComponent& a, const CollisionComponent& b)const
		{
			return check_cir_rec(b, a);
		}
		bool check_cir_rec(const CollisionComponent& a, const CollisionComponent& b)const;
		
		bool checkCircleRectangleCollision(double circleX, double circleY, double radius,double rectLeft,double rectRight,double rectTop,double rectBottom)const
		{
			// 找到矩形上距离圆心最近的点
			double closestX = std::fmax(rectLeft, std::fmin(circleX, rectRight));
			double closestY = std::fmax(rectTop, std::fmin(circleY, rectBottom));

			// 计算圆心到最近点的距离平方
			double distanceX = circleX - closestX;
			double distanceY = circleY - closestY;
			double distanceSquared = (distanceX * distanceX) + (distanceY * distanceY);

			// 判断距离平方是否小于等于半径平方
			return distanceSquared <= (radius * radius);
		}
	};
}