#pragma once
#include<SDL_rect.h>
#include <glm/vec2.hpp>

typedef struct SDL_Texture SDL_Texture;

namespace engine::component
{
    struct SpriteComponent
    {
        enum class SpriteLayer
        {
            Tile=0,
            Object,
            Player,
            Effect,
            None
        };

        enum class AnchorMode
        {
            NONE=0,           // 不指定对齐方式，偏移量通常为 (0,0) 或手动设置
            TOP_LEFT,       // 左上角
            TOP_CENTER,     // 顶部中心
            CENTER,         // 正中心 (几何中心)
            BOTTOM_CENTER,  // 底部中心
        };

        SDL_Texture* texture=nullptr;           //纹理资源
        SDL_Rect src_rect = {0};                //要绘制的纹理部分
        glm::ivec2 size{0,0};                   //精灵尺寸
        SpriteLayer layer = SpriteLayer::None;  //渲染层级
        AnchorMode mode = AnchorMode::NONE;     //对齐方式
        bool is_visible= true;                  //是否需要渲染
    };
}