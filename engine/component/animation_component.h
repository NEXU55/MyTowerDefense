#pragma once
namespace engine::resource
{
    struct AnimationClip;
}
namespace engine::component
{
    struct AnimationComponent
    {
        engine::resource::AnimationClip* current_clip = nullptr;
        int idx_frame = 0;
        Direction dir = Direction::Right;
        double pass_time = 0;
    };
}