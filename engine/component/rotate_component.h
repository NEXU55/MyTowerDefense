#pragma once

namespace engine::component
{
	struct RotateComponent
	{
		bool can_rotated = false;	//是否旋转
		double speed = 0;			//旋转速度
	};
}