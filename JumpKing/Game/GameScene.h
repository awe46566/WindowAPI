#pragma once

#include "Framework/Scene.h"
#include "Texture.h"

class GameScene : public Scene
{
public:
    void Init() override;
    void Render(const RenderContext& context) override;

private:
    Texture _backgroundTexture;
    bool _isBackgroundLoaded = false;
};
