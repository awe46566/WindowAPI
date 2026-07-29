#pragma once

#include <vector>

class Actor;

class Scene
{
public:
    virtual ~Scene();

    virtual void Init() {}
    virtual void Update(float deltaTime);
    virtual void Render(HDC hdc);

    void AddActor(Actor* actor);

protected:
    std::vector<Actor*> _actors;
};
