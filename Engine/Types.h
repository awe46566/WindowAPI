#pragma once

#include <cstdint>

constexpr float PI = 3.14159265358979f;

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

struct HitResult
{
    Vector2 normal;
    float depth = 0.0f;
};

enum class SceneType
{
    Main,
    Game,
    Ending,
    Count
};

enum class ActorType
{
    Player,
};

enum class RenderLayer
{
    Player,
};

enum class JumpState
{
    Ready,      // 점프 가능 상태
    Charging,   // 점프 차징 상태
    AirBorne    // 공중에 있는 상태
};
