#pragma once
#include "Actor.h"

class Background : public Actor
{
	using Super = Actor;
public:
	void Init();

	virtual RenderLayer GetRenderLayer() override { return RenderLayer::Background; }
	virtual ActorType GetActorType() override { return ActorType::Background; }
private:
};

