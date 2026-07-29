#include "pch.h"
#include "framework.h"
#include "Framework/Actor.h"
#include "Framework/Component.h"

Actor::~Actor()
{
    for (Component* component : _components)
    {
        delete component;
    }
    _components.clear();
}

void Actor::Update(float deltaTime)
{
    for (Component* component : _components)
    {
        component->Update(deltaTime);
    }
}

void Actor::Render(HDC hdc)
{
    for (Component* component : _components)
    {
        component->Render(hdc, _position);
    }
}
