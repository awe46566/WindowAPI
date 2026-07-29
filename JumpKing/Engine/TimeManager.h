#pragma once

#include "Engine/Singleton.h"
#include "Engine/Types.h"

class TimeManager : public Singleton<TimeManager>
{
    friend class Singleton<TimeManager>;

public:
    void Init();
    void Update();

    float GetDeltaTime() const { return _deltaTime; }
    uint32 GetFPS() const { return _fps; }

private:
    TimeManager() = default;
    ~TimeManager() = default;

    uint64 _frequency = 0;
    uint64 _previousCount = 0;
    float _deltaTime = 0.0f;

    uint32 _frameCount = 0;
    float _frameTime = 0.0f;
    uint32 _fps = 0;
};
