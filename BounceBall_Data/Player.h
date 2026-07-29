#pragma once
#include "Actor.h"

enum class PlayerState 
{ 
	Idle, 
	Move, 
	FindPath 
};

class Player : public Actor
{
	using Super = Actor;
public:

	void Init();
	virtual void Update(float deltaTime) override;
	virtual void Render(HDC hdc) override;

	void SetPath(const vector<Cell>& path);

	RenderLayer GetRenderLayer() override { return RenderLayer::Player; }
	ActorType GetActorType() override { return ActorType::Player; }
	
private:
	static constexpr float MOVE_DURATION = 0.3f;
	static constexpr float IDLE_ANIM_DURATION = 0.6f;

	PlayerState _state = PlayerState::Idle;
	class SpriteAnimRenderer* _sprite = nullptr;
	bool _facingLeft = false;

	// 셀단위로 이동중일때는 목표가 되는 셀의 좌표와 
	// 실시간으로 보여지는 시각적인 좌표가 다르다.
	// 시각적인 좌표는 _pos (원래 Actor의 좌표로 계산)
	Vector _destPos;	// 물리적인 좌표.
	Vector _startRenderPos;
	float _moveTimer = 0;

	vector<Cell> _path;
	int32 _pathIndex = 0;

	// FSM 구조
	// 본인의 State에 맞는 로직이 수행되어야 한다.
	void changeState(PlayerState s);

	void updateIdle(float dt);
	void updateMove(float dt);
	void updateFindPath(float dt);
	bool moveToNextCell();
};
