#pragma once

#include "Framework/Scene.h"
#include "Engine/Texture.h"
#include "LevelData.h"
#include "Weather.h"
#include "Wind.h"
#include <vector>

class Player;

class GameScene : public Scene
{
public:
    void Init() override;
    void Update(float deltaTime) override;
    void Render(const RenderContext& context) override;
    void DrawCollider(const RenderContext& context);
    void ColliderOnOff();

private:
    const LevelData& CurrentLevel() const { return _levels[_currentLevelIndex]; }
    void CheckLevelTransition();
    void TransitionToLevel(int newIndex, float newY);

    std::vector<LevelData> _levels;
    int _currentLevelIndex = 0;
    Player* _player = nullptr;
    Wind _wind;
    Weather _weather;

    Texture _backgroundTexture;
    Texture _midgroundTexture;
    Texture _foregroundTexture;

    bool _isBackgroundLoaded = false;
    bool _isMidgroundLoaded = false;
    bool _isForegroundLoaded = false;
};
