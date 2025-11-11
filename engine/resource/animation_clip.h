#pragma once
#include<array>
#include<vector>
#include<SDL_rect.h>

typedef struct SDL_Texture SDL_Texture;
namespace engine::resource
{
    struct AnimationClip
    {
        std::array<std::vector<SDL_Rect>, 4> frame;     //四个方向的动画帧序列
        double duration = 0;                            //帧间隔
        SDL_Texture* texture = nullptr;                 //对应纹理
        bool is_loop = false;                           //是否循环播放
    };
}