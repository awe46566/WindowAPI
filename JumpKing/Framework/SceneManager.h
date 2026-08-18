#pragma once

#include "Engine/Singleton.h"
#include "Engine/Types.h"
#include <optional>

class Scene;
struct RenderContext;

class SceneManager : public Singleton<SceneManager>
{
    friend class Singleton<SceneManager>;

public:
    void Init();
    void Cleanup();
    void Update(float deltaTime);
    void Render(const RenderContext& context);

    // 씬 전환을 요청만 해 둔다. 실제 교체는 다음 프레임 Update() 시작 시점에
    // 일어나므로, 지금 실행 중인 씬의 Update 호출 스택 한가운데서 그 씬이
    // 자기 자신을 delete하게 되는 상황을 피할 수 있다.
    void RequestSceneChange(SceneType type);

private:
    SceneManager() = default;
    ~SceneManager() = default;

    Scene* CreateScene(SceneType type);
    void ApplyPendingSceneChange();

    Scene* _currentScene = nullptr;
    std::optional<SceneType> _pendingSceneType;
};
