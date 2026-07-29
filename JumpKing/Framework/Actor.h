#pragma once

#include "Engine/Types.h"

#include <vector>

class Component;

class Actor
{
public:
    virtual ~Actor();

    virtual void Init() {}
    virtual void Update(float deltaTime);
    virtual void Render(HDC hdc);

    const Vector2& GetPosition() const { return _position; }
    void SetPosition(const Vector2& position) { _position = position; }

protected:
    template <typename T>
    T* AddComponent()
    {
        T* component = new T();
        _components.push_back(component);
        return component;
    }

private:
    Vector2 _position;
    std::vector<Component*> _components;
};
