#pragma once

#include "Framework/Scene.h"

// TODO: 다음 단계에서 함께 채운다.
// - Render(): "THE END" 축하 문구 + "PRESS SPACE TO RETURN" 안내를 DrawTextW로 그림
// - Update(): Space 입력 시 SceneManager::RequestSceneChange(SceneType::Main)
class EndingScene : public Scene
{
public:
    void Update(float deltaTime) override;
    void Render(const RenderContext& context) override;
};
