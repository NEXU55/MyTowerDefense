#pragma once
#include"timer.h"
#include"../core/type.h"

namespace engine::component
{
    struct EffectComponent
    {
        Timer timer;        //特效计时器
        Entity owner = -1;
    };
}