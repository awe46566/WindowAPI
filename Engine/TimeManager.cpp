#include "pch.h"
#include "framework.h"
#include "Engine/TimeManager.h"

void TimeManager::Init()
{
    QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&_frequency));
    QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&_previousCount));
}

void TimeManager::Update()
{
    uint64 currentCount = 0;
    QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currentCount));

    _deltaTime =
        static_cast<float>(currentCount - _previousCount) /
        static_cast<float>(_frequency);
    _previousCount = currentCount;

    // 디버거 중단이나 순간적인 지연이 물리 계산에 그대로 전달되지 않게 막습니다.
    constexpr float MAX_DELTA_TIME = 1.0f / 30.0f;
    if (_deltaTime > MAX_DELTA_TIME)
    {
        _deltaTime = MAX_DELTA_TIME;
    }

    ++_frameCount;
    _frameTime += _deltaTime;

    if (_frameTime >= 1.0f)
    {
        _fps = _frameCount;
        _frameCount = 0;
        _frameTime -= 1.0f;
    }
}
