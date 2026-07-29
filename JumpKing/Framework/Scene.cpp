#include "pch.h"
#include "framework.h"
#include "Framework/Scene.h"
#include "Framework/Actor.h"

Scene::~Scene()
{
    for (Actor* actor : _actors)
    {
        delete actor;
    }
    _actors.clear();
}

void Scene::Update(float deltaTime)
{
    for (Actor* actor : _actors)
    {
        actor->Update(deltaTime);
    }
}

void Scene::Render(HDC hdc)
{
    for (Actor* actor : _actors)
    {
        actor->Render(hdc);
    }
}

void Scene::AddActor(Actor* actor)
{
    if (actor == nullptr)
    {
        return;
    }

    actor->Init();
    _actors.push_back(actor);
}
