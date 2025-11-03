#pragma once

namespace engine::core
{
    enum class GameState 
    {
        Title,          ///标题界面
        Playing,        ///正常游戏进行中
        Paused,         ///游戏暂停（通常覆盖菜单界面）
        GameOver,       ///游戏结束界面
    };
}