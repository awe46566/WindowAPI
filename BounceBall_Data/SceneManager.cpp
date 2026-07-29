#include "pch.h"
#include "SceneManager.h"
#include "Scene.h"
#include "GameScene.h"
#include "EditorScene.h"
#include "LobbyScene.h"
#include "PlayerScene.h"

void SceneManager::Init()
{
	_scene = createScene(SceneType::Lobby);
	_scene->Init();
}

void SceneManager::Cleanup()
{
	if (_scene)
	{
		_scene->Cleanup();
		delete _scene;
		_scene = nullptr;
	}
}

void SceneManager::Update(float deltaTime)
{
	// Apply a reserved scene change here, outside of Scene::Update,
	// so a scene can safely request its own replacement mid-update.
	applySceneChange();

	if (_scene)
	{
		_scene->Update(deltaTime); 

		// 모든 Update 끝난후 필요한것들 수행
		_scene->PostUpdate();
		   // ->ChangeScene
		  //              delete _scene;
	}
}

void SceneManager::Render(HDC hdc)
{
	if (_scene)
	{
		_scene->Render(hdc);
	}
}

void SceneManager::ChangeScene(SceneType type)
{
	//delete _scene;
	//_scene = new GameScene();
	_nextSceneType = type;
}

Scene* SceneManager::createScene(SceneType type)
{
	switch (type)
	{
	case SceneType::Lobby:	return new LobbyScene();
	case SceneType::Game:	return new GameScene();
	case SceneType::Editor:	return new EditorScene();
	case SceneType::AStar:	return new PlayerScene();
	}

	return nullptr;
}

void SceneManager::applySceneChange()
{
	if (_nextSceneType == SceneType::Max)
		return;

	if (_scene)
	{
		_scene->Cleanup();
		delete _scene;
	}

	_scene = createScene(_nextSceneType);
	_scene->Init();

	// 전환이 종료되었으니, None 초기화
	_nextSceneType = SceneType::Max;
}