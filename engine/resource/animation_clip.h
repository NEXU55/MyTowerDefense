#pragma once
#include<array>
#include<vector>
#include<SDL_rect.h>

typedef struct SDL_Texture SDL_Texture;
namespace engine::resource
{
    struct AnimationClip
    {
        std::array<std::vector<SDL_Rect>, 4> frame;
        double duration = 0;
        SDL_Texture* texture = nullptr;
        bool is_loop = false;
    };
}