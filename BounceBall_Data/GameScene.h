#pragma once
#include "Scene.h"
#include "UIPanel.h"

class GameScene : public Scene
{
public:
	GameScene();

	virtual void Update(float deltaTime) override;
	virtual void Render(HDC hdc);

	// 죽음 처리
	void Dead(Vector pos);

	// 별을 획득할때마다 호출되는 함수
	void AddStarCount(Vector pos);

protected:
	virtual void createObjects();

	void loadStage();

private:
	// 현재 스테이지 번호
	int32 _currStage = 1;

	// 별과 관련된 변수
	int32 _curStarCount = 0;
	int32 _maxStarCount = 0;

	// 스테이지 클리어시 보여주는 UI Panel
	UIPanel _nextLevelPopup;
};

