#pragma once

#include "Engine/SpriteRenderer.h"
#include "Game/LevelData.h"

struct RenderContext;

class JumpEffect
{
public:
    bool Load();
    void Play(const Vector2& position, PlatformMaterial material);
    void Update(float deltaTime);
    void Render(const RenderContext& context);

private:
    SpriteRenderer _normalSprite;
    SpriteRenderer _snowSprite;
    SpriteRenderer* _activeSprite = nullptr;
    Vector2 _position;
};
