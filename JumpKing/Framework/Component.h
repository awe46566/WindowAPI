#pragma once

#include "Engine/Types.h"

struct RenderContext;

class Component
{
public:
    virtual ~Component() = default;

    virtual void Update(float deltaTime) {}
    virtual void Render(
        const RenderContext& context,
        const Vector2& ownerPosition) {}
};
