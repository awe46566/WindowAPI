#pragma once

#include <cstdint>

using int32 = std::int32_t;
using uint32 = std::uint32_t;
using uint64 = std::uint64_t;

struct Vector2
{
    float x = 0.0f;
    float y = 0.0f;
};

struct Rect
{
    float x = 0.0f;
    float y = 0.0f;
    float width = 0.0f;
    float height = 0.0f;

    float Left() const { return x; }
    float Top() const { return y; }
    float Right() const { return x + width; }
    float Bottom() const { return y + height; }
};

enum class SceneType
{
    Game,
    Count
};

enum class ActorType
{
    Background,
    Foreground,
    Mainground,
    Effect,
    Player,
};

enum class RenderLayer
{
    Background,
    Mainground,
    Player,
    Foreground,
    Effect,

    // 최대 개수
    Count
};


constexpr int32 GAME_SCREEN_WIDTH = 480;
constexpr int32 GAME_SCREEN_HEIGHT = 360;
constexpr int32 WINDOW_SCALE = 2;
constexpr float TARGET_FRAME_RATE = 120.0f;
