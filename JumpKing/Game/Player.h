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
	const vector<PlatformData>* _platforms = nullptr;
	SpriteRenderer* _spriteRenderer = nullptr;
	Collider* _collider = nullptr;	
	JumpState _jumpState = JumpState::Ready;

	Vector2 _velocity;
	Vector2 _jumpDirection = { 0.0f, -1.0f };

	float _jumpChargeTime = 0.0f;
	int _jumpChargeStep = 0;
	
	
};
