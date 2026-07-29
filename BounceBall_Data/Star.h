#pragma once
#include "Actor.h"
#include "IPickupable.h"

class Star : public Actor, public IPickupable
{
public:
	virtual void Init() override;
	virtual RenderLayer GetRenderLayer() override { return RenderLayer::Background; }
	virtual ActorType GetActorType() override { return ActorType::Star; }

	virtual void OnPickedUp(class Ball* ball) override;
};

//class ItemPowerUp : public Actor, public IPickupable
//{
//	virtual void OnPickedUp(class Ball* ball) override;
//};