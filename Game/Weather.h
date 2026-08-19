#pragma once

#include "Engine/Texture.h"
#include "Game/LevelData.h"

struct RenderContext;

class Weather
{
public:
    bool LoadVariant(WeatherType type, int levelIndex);
    void Update(float deltaTime);
    void Render(const RenderContext& context, bool hasWind, float windScrollOffset);

private:
    Texture _frames[4];
    bool _isLoaded = false;
    int _currentFrame = 0;
    float _frameTimer = 0.0f;

    Texture _mask;
    bool _hasMask = false;
};
