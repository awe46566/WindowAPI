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
	void SetPlatforms(const vector<PlatformData>* platforms) { _platforms = platforms; }

private:
	static constexpr float GRAVITY = 900.0f;
	static constexpr float MAX_FALL_SPEED = 600.0f;
	static constexpr float MAX_JUMP_CHARGE_TIME = 0.6f;

	SpriteRenderer* _spriteRenderer = nullptr;
	Collider* _collider = nullptr;	
	JumpState _jumpState = JumpState::Ready;

	float _jumpCharge = 0.0f;
	float _jumpSpeed = 100.0f;
	float _moveSpeed = 70.0f;
	float _verticalVelocity = 0.0f;

	const vector<PlatformData>* _platforms = nullptr;
	
};
