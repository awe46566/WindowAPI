#include "pch.h"
#include "UIManager.h"
#include "ResourceManager.h"
#include "Texture.h"
#include "Game.h"
#include "Scene.h"
#include "SceneManager.h"

void UIManager::Init()
{
	_hpTexture = ResourceManager::GetInstance().GetTexture(L"PlayerHP");
}

void UIManager::Update(float deltaTime)
{
}

void UIManager::Render(HDC hdc)
{
	if (_hpTexture == nullptr)
		return;

	Scene* scene = SceneManager::GetInstance().GetScene();
	if (scene == nullptr)
		return;

}
