#pragma once
#include "Singleton.h"

class SceneManager : public Singleton<SceneManager>
{
	// lobby Scene
	// Game Scene
	// Editor Scene
	friend Singleton<SceneManager>;

public:
	void Init();
	void Cleanup();

	void Update(float deltaTime);
	void Render(HDC hdc);

	// Reserve a scene change; applied at the start of the next Update
	void ChangeScene(SceneType type);

	class Scene* GetScene() const { return _scene; }

private:
	SceneManager() = default;
	~SceneManager() = default;

	class Scene* createScene(SceneType type);
	void applySceneChange();

public:
	// update, render 돌고있는 현재 출력중인 Scene
	class Scene* _scene = nullptr;

	// Scene Update중에 다른Scene으로 전환이 일어나면,
	// 현재 Scene 포인터를 삭제해야해서, 예약시스템으로 변환
	SceneType _nextSceneType = SceneType::Max;
};

