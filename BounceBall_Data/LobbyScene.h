#pragma once
#include "Scene.h"
#include "UIPanel.h"

class LobbyScene : public Scene
{
public:
	LobbyScene();
	virtual void Update(float deltaTime) override;
	virtual void Render(HDC hdc);

protected:
	virtual void createObjects();

private:
	class Texture* _bg = nullptr;

	UIPanel _panel;
};

