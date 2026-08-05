#include "pch.h"
#include "framework.h"
#include "Engine/RenderContext.h"
#include "Engine/ResourceCatalog.h"
#include "Engine/DebugRenderer.h"
#include "Engine/InputManager.h"
#include "Game/GameScene.h"
#include "Game/LevelData.h"
#include "Player.h"

namespace
{
    bool LoadLayerTexture(const std::string& imageId, Texture& texture)
    {
        // 레벨에 해당 레이어가 없는 경우는 오류가 아니라 그리지 않는 레이어입니다.
        if (imageId.empty())
        {
            return false;
        }

        const ImageResource* image =
            ResourceCatalog::GetInstance().FindImage(imageId);
        return image != nullptr &&
            texture.Load(image->path, image->rows, image->columns);
    }

    void RenderTextureLayer(
        Texture& texture,
        bool isLoaded,
        const RenderContext& context)
    {
        if (!isLoaded)
        {
            return;
        }

        const D2D1_RECT_F sourceRect = D2D1::RectF(
            0.0f,
            0.0f,
            static_cast<float>(texture.GetWidth()),
            static_cast<float>(texture.GetHeight()));
        texture.Render(context, Vector2{}, sourceRect);
    }
}

void GameScene::Init()
{   
    wchar_t executablePath[MAX_PATH]{};
    const DWORD pathLength = GetModuleFileNameW(
        nullptr,
        executablePath,
        ARRAYSIZE(executablePath));

    if (pathLength > 0 && pathLength < ARRAYSIZE(executablePath))
    {
        const fs::path levelPath =
            fs::path(executablePath).parent_path() / L".." / L".." / L"JumpKing" / L"Data" / L"levels.json";

        if (LoadLevelData(levelPath.lexically_normal(), "level_00", _currentLevel))
        {
            _isBackgroundLoaded = LoadLayerTexture(_currentLevel.layers.background, _backgroundTexture);
            _isMidgroundLoaded = LoadLayerTexture(_currentLevel.layers.midground, _midgroundTexture);
            _isForegroundLoaded = LoadLayerTexture(_currentLevel.layers.foreground, _foregroundTexture);
        }
    }

    Player* player = new Player();
    player->SetPlatforms(&_currentLevel.platforms);
    player->SetPosition(_currentLevel.hasSpawn ? _currentLevel.spawn : Vector2{ 230.0f, 286.0f });
    AddActor(player);
}

void GameScene::Render(const RenderContext& context)
{
    // 먼저 그린 이미지가 뒤에 놓이고, 나중에 그린 이미지가 앞을 덮습니다.
    RenderTextureLayer(_backgroundTexture, _isBackgroundLoaded, context);
    RenderTextureLayer(_midgroundTexture, _isMidgroundLoaded, context);  

    Scene::Render(context);

    // Foreground는 플레이어보다 앞에 보여야 하므로 Actor 렌더링 뒤에 그립니다.
    RenderTextureLayer(_foregroundTexture, _isForegroundLoaded, context);
    DrawCollider(context);
    ColliderOnOff();
}

void GameScene::DrawCollider(const RenderContext& context)
{
    for (PlatformData& platform : _currentLevel.platforms)
    {
        if (platform.hasSlope)
            continue;

        DebugRenderer::DrawRect(context, platform.bounds, D2D1::ColorF(D2D1::ColorF::Red));
    }
}

void GameScene::ColliderOnOff()
{
    InputManager& input = InputManager::GetInstance();
    if (input.GetButtonDown(KeyType::F1))
    {
        DebugRenderer::ToggleColliderLine();
    }
}

