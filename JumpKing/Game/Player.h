#pragma once
#include "Framework/Actor.h"
#include "Game/LevelData.h"

class SpriteRenderer;
class ColliderAABB;

// player_king.png의 row 순서와 그대로 대응한다 (Idle=row0, Move=row1, ...).
enum class PlayerAnimState
{
	Idle,
	Move,
	Collision,
	Charge,
	Up,
	Down,
	Hurt
};

class Player : public Actor
{

public:
	virtual void Init() override;
	virtual void Update(float deltaTime) override;
	virtual void Render(const RenderContext& context) override;

	virtual RenderLayer GetRenderLayer() override { return RenderLayer::Player; }
	virtual ActorType GetActorType() override { return ActorType::Player; }

	void Move(float deltaTime);
	void ApplyGravity(float deltaTime);
	void UpdateJump(float deltaTime);
	void UpdateNoclip(float deltaTime);
	void StartJump();
	void OnLanded();
	void ChargingDirection();
	void HorizontalCollision(Vector2& nextPosition);
	void VerticalCollision(Vector2& nextPosition, float deltaTime);
	void UpdateAnimation(float deltaTime);
	void SetPlatforms(const vector<PlatformData>* platforms) { _platforms = platforms; }

	void ApplySlopeSlide(float deltaTime);

	void ApplyWind(float deltaTime);
	void SetWindForceX(float forceX) { _windForceX = forceX; }

private:
	float GetSlopeSurfaceY(const PlatformData& platform, float x) const;
	bool ResolveSlopeCollision(const PlatformData& platform, Vector2& nextPosition, float deltaTime);

	const vector<PlatformData>* _platforms = nullptr;
	SpriteRenderer* _spriteRenderer = nullptr;
	ColliderAABB* _collider = nullptr;
	JumpState _jumpState = JumpState::Ready;

	float _jumpAngle = 0.0f;   // 0=위, +=오른쪽, -=왼쪽

	Vector2 _velocity;

	float _jumpChargeTime = 0.0f;
	int _jumpChargeStep = 0;
	bool _isMoveInputPressed = false;

	PlayerAnimState _animState = PlayerAnimState::Idle;
	bool _collisionFlash = false;

	// 현재 서 있는 바닥 정보. 평지면 _groundSlope는 {0,0}.
	bool _isGrounded = false;
	PlatformMaterial _groundMaterial = PlatformMaterial::Land;
	Vector2 _groundSlope{};

	bool _noclip = false;

	float _windForceX = 0.0f;
};
