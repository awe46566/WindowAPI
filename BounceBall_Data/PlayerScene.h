#pragma once
#include "Scene.h"
#include "DebugAStar.h"

class PlayerScene : public Scene
{
	using Super = Scene;
public:
	PlayerScene();
	virtual void Update(float deltaTime) override;
	virtual void Render(HDC hdc) override;

	// 길찾기 수행하는 로직
	bool FindPath(Cell start, Cell end, vector<Cell>& findPath, int32 maxDepth = 10);

	// 해당셀에 Block이 있는지 확인
	bool CanMoveCell(const Cell& cell);

protected:
	virtual void createObjects() override;

	void loadStage();

private:

	Vector _selectorPos;
	bool _showSelector = false;
	class Texture* _selectorTexture = nullptr;

	// for debug
	DebugAStar _debugAStar;
};

