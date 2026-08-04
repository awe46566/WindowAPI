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
	void StartJump(float deltaTime);
	void OnLanded();
	void SetPlatforms(const vector<PlatformData>* platforms) { _platforms = platforms; }

private:
	static constexpr float GRAVITY = 900.0f;
	static constexpr float MAX_FALL_SPEED = 600.0f;
	static constexpr float MAX_JUMP_SPEED = 500.0f;
	static constexpr float MIN_JUMP_SPEED = 250.0f;
	static constexpr float MAX_JUMP_CHARGE_TIME = 0.6f;
	static constexpr int MAX_CHARGE_STEP = 35;

	SpriteRenderer* _spriteRenderer = nullptr;
	Collider* _collider = nullptr;	
	JumpState _jumpState = JumpState::Ready;

	float _jumpChargeTime = 0.0f;
	int _jumpChargeStep = 0;
	float _moveSpeed = 70.0f;
	float _verticalVelocity = 0.0f;

	const vector<PlatformData>* _platforms = nullptr;
	
};
