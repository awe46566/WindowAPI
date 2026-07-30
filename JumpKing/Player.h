#pragma once
#include "Framework/Actor.h"

class Player : public Actor
{
	
public:
	virtual void Init() override;
	virtual void Update(float deltaTime) override;

	virtual RenderLayer GetRenderLayer() override { return RenderLayer::Background; }
	virtual ActorType GetActorType() override { return ActorType::Player; }

};