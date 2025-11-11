#pragma once
#include<glm/vec2.hpp>

namespace engine::component
{
    struct TransformComponent
    {
        glm::dvec2 position = {0,0};        //位置
        glm::dvec2 scale = { 1,1 };         //
        double angle=0;                     //角度
    };
}