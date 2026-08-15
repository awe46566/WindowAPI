#include "pch.h"
#include "framework.h"
#include "Engine/RenderContext.h"
#include "Engine/ResourceCatalog.h"
#include "Engine/DebugRenderer.h"
#include "Engine/InputManager.h"
#include "Engine/GameConstants.h"
using namespace GameConstants;
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

        if (LoadAllLevelData(levelPath.lexically_normal(), _levels))
        {
            _isBackgroundLoaded = LoadLayerTexture(CurrentLevel().layers.background, _backgroundTexture);
            _isMidgroundLoaded = LoadLayerTexture(CurrentLevel().layers.midground, _midgroundTexture);
            _isForegroundLoaded = LoadLayerTexture(CurrentLevel().layers.foreground, _foregroundTexture);
            _weather.LoadVariant(CurrentLevel().weather, CurrentLevel().index);
        }
    }

    Player* player = new Player();
    if (!_levels.empty())
    {
        player->SetPlatforms(&CurrentLevel().platforms);
        player->SetPosition(CurrentLevel().hasSpawn ? CurrentLevel().spawn : Vector2{ 230.0f, 286.0f });
    }
    else
    {
        player->SetPosition(Vector2{ 230.0f, 286.0f });
    }
    _player = player;
    AddActor(player);
}

void GameScene::Update(float deltaTime)
{
    _wind.Update(deltaTime);
    _weather.Update(deltaTime);

    if (!_levels.empty())
    {
        _player->SetWindForceX(CurrentLevel().hasWind
            ? _wind.GetForce() * WIND_FORCE_ACCEL
            : 0.0f);
    }

    Scene::Update(deltaTime);
    CheckLevelTransition();
}

void GameScene::CheckLevelTransition()
{
    // TODO: _player->GetPosition().y 가 0 또는 SCREEN_HEIGHT를
    // 벗어났는지 확인하고, 벗어났다면 TransitionToLevel(newIndex, newY)를 호출한다.
    // - 위로 나감(y < 0) -> index + 1, 새 y = y + SCREEN_HEIGHT
    // - 아래로 나감(y > SCREEN_HEIGHT) -> index - 1, 새 y = y - SCREEN_HEIGHT
    // - _levels 범위를 벗어나는 index(맨 위/맨 아래 레벨)는 전환하지 않는다.
    if (_player->GetPosition().y < 0.0f)
    {     
        float nextY = _player->GetPosition().y + SCREEN_HEIGHT;
        TransitionToLevel(_currentLevelIndex + 1, nextY);
    }
    else if (_player->GetPosition().y > SCREEN_HEIGHT)
    {
        float nextY = _player->GetPosition().y - SCREEN_HEIGHT;
        TransitionToLevel(_currentLevelIndex - 1, nextY);
    }

}

void GameScene::TransitionToLevel(int newIndex, float newY)
{
    // TODO: _currentLevelIndex를 newIndex로 바꾸고, CurrentLevel()의
    // background/midground/foreground를 LoadLayerTexture로 다시 로드해
    // _isXLoaded 플래그와 함께 갱신한다. _player->SetPlatforms(&CurrentLevel().platforms)로
    // 새 레벨의 플랫폼을 연결하고, 플레이어 x는 유지한 채 y만 newY로 재배치한다.
    _currentLevelIndex = newIndex;

    _isBackgroundLoaded = LoadLayerTexture(CurrentLevel().layers.background, _backgroundTexture);
    _isMidgroundLoaded = LoadLayerTexture(CurrentLevel().layers.midground, _midgroundTexture);
    _isForegroundLoaded = LoadLayerTexture(CurrentLevel().layers.foreground, _foregroundTexture);
    _weather.LoadVariant(CurrentLevel().weather, CurrentLevel().index);

    Vector2 position = _player->GetPosition();
    position.y = newY;

    _player->SetPosition(position);
    _player->SetPlatforms(&CurrentLevel().platforms);    
}

void GameScene::Render(const RenderContext& context)
{
    // 먼저 그린 이미지가 뒤에 놓이고, 나중에 그린 이미지가 앞을 덮습니다.
    RenderTextureLayer(_backgroundTexture, _isBackgroundLoaded, context);
    RenderTextureLayer(_midgroundTexture, _isMidgroundLoaded, context);  

    Scene::Render(context);

    _weather.Render(context, CurrentLevel().hasWind, _wind.GetScrollOffset());
    // Foreground는 플레이어보다 앞에 보여야 하므로 Actor 렌더링 뒤에 그립니다.
    RenderTextureLayer(_foregroundTexture, _isForegroundLoaded, context);
    DrawCollider(context);
    ColliderOnOff();
}

void GameScene::DrawCollider(const RenderContext& context)
{
    if (_levels.empty())
        return;

    for (const PlatformData& platform : CurrentLevel().platforms)
    {
        if (platform.hasSlope)
        {
            Vector2 left, right;
            GetSlopeEndpoints(platform, left, right);
            DebugRenderer::DrawLine(
                context,
                left,
                right,
                IsSlopeFloor(platform) ? D2D1::ColorF(D2D1::ColorF::Lime) : D2D1::ColorF(D2D1::ColorF::Cyan));
            continue;
        }

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

