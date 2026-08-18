#include "pch.h"
#include "framework.h"
#include "Framework/SceneManager.h"
#include "Framework/Scene.h"
#include "Game/MainScene.h"
#include "Game/GameScene.h"
#include "Game/EndingScene.h"

void SceneManager::Init()
{
    _currentScene = CreateScene(SceneType::Main);
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
    ApplyPendingSceneChange();

    if (_currentScene != nullptr)
    {
        _currentScene->Update(deltaTime);
    }
}

void SceneManager::Render(const RenderContext& context)
{
    if (_currentScene != nullptr)
    {
        _currentScene->Render(context);
    }
}

void SceneManager::RequestSceneChange(SceneType type)
{
    _pendingSceneType = type;
}

void SceneManager::ApplyPendingSceneChange()
{
    if (!_pendingSceneType.has_value())
    {
        return;
    }

    const SceneType type = *_pendingSceneType;
    _pendingSceneType.reset();

    delete _currentScene;
    _currentScene = CreateScene(type);
    if (_currentScene != nullptr)
    {
        _currentScene->Init();
    }
}

Scene* SceneManager::CreateScene(SceneType type)
{
    switch (type)
    {
    case SceneType::Main:
        return new MainScene();
    case SceneType::Game:
        return new GameScene();
    case SceneType::Ending:
        return new EndingScene();
    default:
        return nullptr;
    }
}
