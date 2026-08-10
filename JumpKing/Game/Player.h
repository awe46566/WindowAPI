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
	void StartJump();
	void OnLanded();
	void ChargingDirection();
	void HorizontalCollision(Vector2& nextPosition);
	void VerticalCollision(Vector2& nextPosition);
	void UpdateAnimation(float deltaTime);
	void SetPlatforms(const vector<PlatformData>* platforms) { _platforms = platforms; }

private:
	// TODO(user): 슬로프 표면 y 계산 및 충돌 스냅. 설계는 계획 문서 참고.
	float GetSlopeSurfaceY(const PlatformData& platform, float x) const;
	bool ResolveSlopeCollision(const PlatformData& platform, Vector2& nextPosition);

	const vector<PlatformData>* _platforms = nullptr;
	SpriteRenderer* _spriteRenderer = nullptr;
	ColliderAABB* _collider = nullptr;
	JumpState _jumpState = JumpState::Ready;

	Vector2 _velocity;
	Vector2 _jumpDirection = { 0.0f, -1.0f };

	float _jumpChargeTime = 0.0f;
	int _jumpChargeStep = 0;

	PlayerAnimState _animState = PlayerAnimState::Idle;
	bool _collisionFlash = false;

	// 현재 서 있는 바닥 정보. 평지면 _groundSlope는 {0,0}.
	bool _isGrounded = false;
	PlatformMaterial _groundMaterial = PlatformMaterial::Land;
	Vector2 _groundSlope{};
};
