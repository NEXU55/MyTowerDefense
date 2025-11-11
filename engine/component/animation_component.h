#pragma once
namespace engine::resource
{
    struct AnimationClip;
}
namespace engine::component
{
    //动画组件，用于确定帧序列和精灵组件的源矩形是哪一帧
    struct AnimationComponent
    {
        engine::resource::AnimationClip* current_clip = nullptr;    //帧序列
        int idx_frame = 0;                                          //当前帧索引
        Direction dir = Direction::Right;                           //方向
        double pass_time = 0;                                       //距上一帧播放时间
    };
}