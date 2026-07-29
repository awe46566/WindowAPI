#pragma once

#include "Scene.h"

class EditorScene : public Scene
{
public:
	EditorScene();

	virtual void Update(float deltaTime) override;
	virtual void Render(HDC hdc);

protected:
	virtual void createObjects();
	Vector cellCenter(Cell cell);

	void saveStage();
	void loadStage();

	void setPreviewType(ActorType type);

private:
	ActorType _previewType = ActorType::Block;
	Actor* _preview = nullptr;
};

