#pragma once

#include <cstdint>

using int32 = std::int32_t;
using uint32 = std::uint32_t;
using uint64 = std::uint64_t;

#define SMALL_NUMBER			(1.e-8f)

struct Vector2
{
    float x = 0.0f;
    float y = 0.0f;

    float Length()
    {
        return sqrt(x * x + y * y);
    }

    void Normalize()
    {
        float length = Length();
        if (length < SMALL_NUMBER)
            return;

        x /= length;
        y /= length;
    }
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

enum class JumpState
{
    Ready,      // 점프 가능 상태
    Charging,   // 점프 차징 상태
    AirBorne    // 공중에 있는 상태
};
