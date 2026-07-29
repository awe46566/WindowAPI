#include "pch.h"
#include "LobbyScene.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "Texture.h"
#include "UIButton.h"
#include "UIImage.h"

LobbyScene::LobbyScene()
{
	_sceneType = SceneType::Lobby;
}

void LobbyScene::Update(float deltaTime)
{
	Scene::Update(deltaTime);

	if (InputManager::GetInstance().GetButtonDown(KeyType::F1))
	{
		//Editor
		SceneManager::GetInstance().ChangeScene(SceneType::Editor);
	}

	if (InputManager::GetInstance().GetButtonDown(KeyType::F2))
	{
		//Game
		SceneManager::GetInstance().ChangeScene(SceneType::Game);
	}

	_panel.Update(deltaTime);
}

void LobbyScene::Render(HDC hdc)
{
	Scene::Render(hdc);

	if (_bg)
	{
		_bg->Render(hdc, Vector(0,0));
	}

	_panel.Render(hdc);
}

void LobbyScene::createObjects()
{
	_bg = ResourceManager::GetInstance().GetTexture(L"Lobby_BG");
	_bg->SetApplyCenter(false);

	// UI 관련 요소를 만들자.
	UIImage* title = new UIImage();
	title->Init(L"Title", Vector(625, 80));
	_panel.AddWidget(title);

	// GameButton
	UIButton* playButton = new UIButton();
	playButton->Init(L"Play", Vector(245, 367));
	playButton->SetOnClick([]() 
		{
			//Game
			SceneManager::GetInstance().ChangeScene(SceneType::Game);
		});
	_panel.AddWidget(playButton);

	// EditorButton
	UIButton* editorButton = new UIButton();
	editorButton->Init(L"Editor", Vector(177, 552));
	editorButton->SetOnClick([]()
		{
			//EditorScene
			SceneManager::GetInstance().ChangeScene(SceneType::Editor);
		});
	_panel.AddWidget(editorButton);

	// AStarButton
	UIButton* aStarButton = new UIButton();
	aStarButton->Init(L"AStar", Vector(600, 367));
	aStarButton->SetOnClick([]()
		{
			//PlayerScene
			SceneManager::GetInstance().ChangeScene(SceneType::AStar);
		});
	_panel.AddWidget(aStarButton);
}