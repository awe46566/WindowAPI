#pragma once
#include "Framework/Actor.h"

class SpriteRenderer;

class Player : public Actor
{
	
public:
	virtual void Init() override;
	virtual void Update(float deltaTime) override;

	virtual RenderLayer GetRenderLayer() override { return RenderLayer::Player; }
	virtual ActorType GetActorType() override { return ActorType::Player; }

private:
	SpriteRenderer* _spriteRenderer = nullptr;
};
