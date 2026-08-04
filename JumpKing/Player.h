#pragma once
#include "Framework/Actor.h"

class SpriteRenderer;
class Collider;
struct PlatformData;

class Player : public Actor
{
	
public:
	virtual void Init() override;
	virtual void Update(float deltaTime) override;

	virtual RenderLayer GetRenderLayer() override { return RenderLayer::Player; }
	virtual ActorType GetActorType() override { return ActorType::Player; }

	void Move(float deltaTime);
	void ApplyGravity(float deltaTime);	
	void UpdateJump(float deltaTime);
	void StartJump();
	void OnLanded();
	void ChargingDirection();
	void SetPlatforms(const vector<PlatformData>* platforms) { _platforms = platforms; }

private:
	static constexpr float MOVE_SPEED = 70.0f;
	static constexpr float GRAVITY = 900.0f;
	static constexpr float MAX_FALL_SPEED = 600.0f;
	static constexpr float MAX_JUMP_SPEED = 600.0f;
	static constexpr float MIN_JUMP_SPEED = 100.0f;
	static constexpr float MAX_JUMP_CHARGE_TIME = 0.6f;
	static constexpr int MAX_CHARGE_STEP = 35;

	const vector<PlatformData>* _platforms = nullptr;
	SpriteRenderer* _spriteRenderer = nullptr;
	Collider* _collider = nullptr;	
	JumpState _jumpState = JumpState::Ready;

	Vector2 _velocity;
	Vector2 _jumpDirection = { 0.0f, -1.0f };

	float _jumpChargeTime = 0.0f;
	int _jumpChargeStep = 0;
	
	
};
