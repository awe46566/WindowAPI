#pragma once

#include <vector>

class Actor;
struct RenderContext;

class Scene
{
public:
    virtual ~Scene();

    virtual void Init() {}
    virtual void Update(float deltaTime);
    virtual void Render(const RenderContext& context);

    void AddActor(Actor* actor);

protected:
    std::vector<Actor*> _actors;
};
