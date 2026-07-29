#pragma once
#include "Actor.h"

class Block : public Actor
{
public:
	virtual void Init() override;
	virtual RenderLayer GetRenderLayer() override { return RenderLayer::Background; }
	virtual ActorType GetActorType() override { return ActorType::Block; }
};

