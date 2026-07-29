#pragma once

class Component
{
public:
    virtual ~Component() = default;

    virtual void Update(float deltaTime) {}
    virtual void Render(HDC hdc, const Vector2& ownerPosition) {}
};
