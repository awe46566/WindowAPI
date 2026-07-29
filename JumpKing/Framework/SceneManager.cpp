#include "pch.h"
#include "framework.h"
#include "Framework/SceneManager.h"
#include "Framework/Scene.h"
#include "Game/GameScene.h"

void SceneManager::Init()
{
    _currentScene = CreateScene(SceneType::Game);
    if (_currentScene != nullptr)
    {
        _currentScene->Init();
    }
}

void SceneManager::Cleanup()
{
    delete _currentScene;
    _currentScene = nullptr;
}

void SceneManager::Update(float deltaTime)
{
    if (_currentScene != nullptr)
    {
        _currentScene->Update(deltaTime);
    }
}

void SceneManager::Render(HDC hdc)
{
    if (_currentScene != nullptr)
    {
        _currentScene->Render(hdc);
    }
}

Scene* SceneManager::CreateScene(SceneType type)
{
    switch (type)
    {
    case SceneType::Game:
        return new GameScene();
    default:
        return nullptr;
    }
}
