#pragma once

#include "pch.h"
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

    // 순수 가상함수. Actor 타입과 Layer 순서를 알려줘야 함.
    virtual RenderLayer GetRenderLayer() = 0;
    virtual ActorType GetActorType() = 0;

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
