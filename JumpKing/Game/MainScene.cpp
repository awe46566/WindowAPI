#include "pch.h"
#include "framework.h"
#include "Game/MainScene.h"

void MainScene::Init()
{
    // TODO: ResourceCatalog::GetInstance().FindImage("lobby_logo") -> _logoTexture.Load(...)
}

void MainScene::Update(float deltaTime)
{
    // TODO: InputManager::GetInstance().GetButtonDown(KeyType::Space) 확인 후
    // SceneManager::GetInstance().RequestSceneChange(SceneType::Game)
}

void MainScene::Render(const RenderContext& context)
{
    // TODO: 로고 텍스처 렌더링 + "PRESS SPACE TO START" 텍스트 렌더링
}
