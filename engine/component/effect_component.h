#pragma once
#include"timer.h"
#include"../core/type.h"

namespace engine::component
{
    struct EffectComponent
    {
        Timer timer;
        Entity owner = -1;
    };
}