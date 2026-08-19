#pragma once

#include "Framework/Scene.h"
#include "Engine/Texture.h"
#include "Engine/Types.h"

// TODO: 다음 단계에서 함께 채운다.
// - Init(): ResourceCatalog에서 "lobby_logo" 이미지를 찾아 _logoTexture에 Load
// - Render(): 로고를 그리고, "PRESS SPACE TO START" 안내 문구를 DrawTextW로 그림
// - Update(): Space 입력 시 SceneManager::RequestSceneChange(SceneType::Game)
class MainScene : public Scene
{
public:
    void Init() override;
    void Update(float deltaTime) override;
    void Render(const RenderContext& context) override;

private:
    Texture _logoTexture;
    bool _isLogoLoaded = false;

    float _blinkTimer = 0.0f;
    bool _isTextVisible = true;

    Microsoft::WRL::ComPtr<IDWriteTextFormat> _titleTextFormat;
    Microsoft::WRL::ComPtr<IDWriteFontCollection1> _fontCollection;
};
