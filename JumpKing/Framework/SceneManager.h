#pragma once

#include "Engine/Singleton.h"
#include "Engine/Types.h"

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

private:
    SceneManager() = default;
    ~SceneManager() = default;

    Scene* CreateScene(SceneType type);

    Scene* _currentScene = nullptr;
};
