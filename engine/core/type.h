#pragma once
#include<bitset> //用于std::uint32_t std::bitset

//定义实体类、签名、组件种类、索引
using Entity = std::uint32_t;
const Entity MAX_ENTITY = 8192;
using ComponentType = std::uint8_t;
const ComponentType MAX_COMPONENT = 32;
using Signature = std::bitset<MAX_COMPONENT>;
using Index = size_t;

enum class Direction
{
    Up = 0,
    Down,
    Left,
    Right
};

enum class ActionState 
{
    INACTIVE,           //动作未激活
    PRESSED_THIS_FRAME, //动作在本帧刚刚被按下
    HELD_DOWN,          //动作被持续按下
    RELEASED_THIS_FRAME //动作在本帧刚刚被释放
};