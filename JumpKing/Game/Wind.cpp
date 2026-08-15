#include "pch.h"
#include "Wind.h"
#include "Engine/GameConstants.h"
using namespace GameConstants;

void Wind::Update(float deltaTime)
{
    // TODO: Python Wind.calculate_wind()를 참고해 아래 세 가지를 채운다.
    // (원본: wind = sin(wind_var) * 6.25 / wind_var += pi/500 / self.x += wind)
    //
    // 1) _force = sinf(_phase) * WIND_AMPLITUDE 로 현재 바람 세기를 구한다.
    // 2) _phase += WIND_PHASE_SPEED * deltaTime 로 위상을 전진시킨다.
    //    (원본은 deltaTime 없이 프레임마다 고정값을 더했지만, 이 프로젝트는 가변 deltaTime을
    //     쓰므로 반드시 deltaTime을 곱해야 프레임률과 무관하게 같은 속도로 진동한다.)
    // 3) _scrollOffset += _force * deltaTime 로 스크롤 오프셋을 누적한다.
    //    (Weather가 나중에 이 값을 화면 스크롤 위치로 재사용한다.)
    
    _force = sinf(_phase) * WIND_AMPLITUDE;
    _phase += WIND_PHASE_SPEED * deltaTime;
    _scrollOffset += _force * deltaTime;
}
