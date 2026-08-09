#pragma once

#include "Framework/Scene.h"
#include "Engine/Texture.h"
#include "LevelData.h"
#include <vector>

class GameScene : public Scene
{
public:
    void Init() override;
    void Render(const RenderContext& context) override;
    void DrawCollider(const RenderContext& context);
    void ColliderOnOff();

private:
    const LevelData& CurrentLevel() const { return _levels[_currentLevelIndex]; }

    std::vector<LevelData> _levels;
    int _currentLevelIndex = 0;

    Texture _backgroundTexture;
    Texture _midgroundTexture;
    Texture _foregroundTexture;

    bool _isBackgroundLoaded = false;
    bool _isMidgroundLoaded = false;
    bool _isForegroundLoaded = false;
};
