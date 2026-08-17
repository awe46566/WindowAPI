#include "pch.h"
#include "Wind.h"
#include "Engine/GameConstants.h"
using namespace GameConstants;

void Wind::Update(float deltaTime)
{
    _force = sinf(_phase) * WIND_AMPLITUDE;
    _phase += WIND_PHASE_SPEED * deltaTime;
    _scrollOffset += _force * deltaTime;
}
