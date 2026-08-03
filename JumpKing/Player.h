#pragma once
#include "Framework/Actor.h"

class SpriteRenderer;
class Collider;

class Player : public Actor
{
	
public:
	virtual void Init() override;
	virtual void Update(float deltaTime) override;

	virtual RenderLayer GetRenderLayer() override { return RenderLayer::Player; }
	virtual ActorType GetActorType() override { return ActorType::Player; }

	void Move(float deltaTime);

private:
	SpriteRenderer* _spriteRenderer = nullptr;
	Collider* _collider = nullptr;
	float _moveSpeed = 70.0f;
};
