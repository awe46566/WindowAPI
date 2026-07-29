#pragma once

#include "Framework/Scene.h"

class GameScene : public Scene
{
public:
    void Init() override;
    void Render(HDC hdc) override;
};
