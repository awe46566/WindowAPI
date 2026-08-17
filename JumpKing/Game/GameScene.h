#pragma once

#include "Framework/Scene.h"
#include "Engine/Texture.h"
#include "LevelData.h"
#include "Weather.h"
#include "Wind.h"
#include "Scrolling.h"
#include "Props.h"
#include "HiddenWalls.h"
#include "JumpEffect.h"
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
    void CheckLevelTransition(float deltaTime);
    void TransitionToLevel(int newIndex, float newY, float deltaTime);

    std::vector<LevelData> _levels;
    int _currentLevelIndex = 0;
    Player* _player = nullptr;
    Wind _wind;
    std::vector<Weather> _weathers;
    std::vector<ScrollingClouds> _scrollingClouds;
    std::vector<Props> _props;
    std::vector<HiddenWalls> _hiddenWalls;
    JumpEffect _jumpEffect;

    // 레벨 전환마다 디스크에서 다시 로드하면 그 프레임이 순간적으로 느려져
    // (deltaTime 스파이크) 다음 프레임에 충돌이 씹히므로, 레벨별로 한 번만 로드해 둔다.
    std::vector<Texture> _backgroundTextures;
    std::vector<Texture> _midgroundTextures;
    std::vector<Texture> _foregroundTextures;

    std::vector<bool> _isBackgroundLoaded;
    std::vector<bool> _isMidgroundLoaded;
    std::vector<bool> _isForegroundLoaded;
};
