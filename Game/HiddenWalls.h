#pragma once

#include "Engine/SpriteRenderer.h"
#include "Game/LevelData.h"
#include <vector>

struct RenderContext;

class HiddenWalls
{
public:
    bool Load(const std::vector<PropData>& data);
    void Update(float deltaTime, const Rect& playerBounds);
    void Render(const RenderContext& context);

private:
    struct HiddenWallInstance
    {
        SpriteRenderer sprite;
        Vector2 position;
        float opacity = 1.0f;
    };

    std::vector<HiddenWallInstance> _hiddenWalls;
};
