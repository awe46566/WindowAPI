#pragma once
#include "Framework/Actor.h"

class Player : public Actor
{
public:
	void Init() override;
	void Render(HDC hdc) override;

};