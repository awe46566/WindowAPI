#pragma once

#include "Framework/Scene.h"

// TODO: 다음 단계에서 채운다.
// - Init(): MainScene::Init()의 Pixolde 폰트 로딩 블록(IDWriteFactory5 -> CreateFontSetBuilder
//   -> CreateFontFileReference -> AddFontFile -> CreateFontSet -> CreateFontCollectionFromFontSet
//   -> CreateTextFormat)을 그대로 옮겨와 _titleTextFormat을 채운다.
// - Render(): "THE END" 축하 문구 + "PRESS SPACE TO RETURN" 안내를 DrawText로 그림
// - Update(): Space 입력 시 SceneManager::RequestSceneChange(SceneType::Main)
class EndingScene : public Scene
{
public:
    void Init() override;
    void Update(float deltaTime) override;
    void Render(const RenderContext& context) override;

private:
    Microsoft::WRL::ComPtr<IDWriteTextFormat> _titleTextFormat;
    Microsoft::WRL::ComPtr<IDWriteFontCollection1> _fontCollection;
};
