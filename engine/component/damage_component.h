#pragma once

namespace engine::component
{
    struct DamageComponent
    {
        double damage = 0;
        double damage_max = 0;
        bool one_shot = true;
    };
}