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

constexpr int32 GAME_SCREEN_WIDTH = 800;
constexpr int32 GAME_SCREEN_HEIGHT = 720;
constexpr float TARGET_FRAME_RATE = 120.0f;
