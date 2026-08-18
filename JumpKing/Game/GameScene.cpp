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
            const size_t levelCount = _levels.size();
            _backgroundTextures.resize(levelCount);
            _midgroundTextures.resize(levelCount);
            _foregroundTextures.resize(levelCount);
            _isBackgroundLoaded.resize(levelCount, false);
            _isMidgroundLoaded.resize(levelCount, false);
            _isForegroundLoaded.resize(levelCount, false);
            _weathers.resize(levelCount);
            _scrollingClouds.resize(levelCount);
            _props.resize(levelCount);
            _hiddenWalls.resize(levelCount);

            for (size_t i = 0; i < levelCount; ++i)
            {
                _isBackgroundLoaded[i] = LoadLayerTexture(_levels[i].layers.background, _backgroundTextures[i]);
                _isMidgroundLoaded[i] = LoadLayerTexture(_levels[i].layers.midground, _midgroundTextures[i]);
                _isForegroundLoaded[i] = LoadLayerTexture(_levels[i].layers.foreground, _foregroundTextures[i]);
                _weathers[i].LoadVariant(_levels[i].weather, _levels[i].index);
                _scrollingClouds[i].Load(_levels[i].scrolling, _levels[i].birds);
                _props[i].Load(_levels[i].props);
                _hiddenWalls[i].Load(_levels[i].hiddenWalls);
            }
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

    _jumpEffect.Load();
}

void GameScene::Update(float deltaTime)
{
    _wind.Update(deltaTime);

    if (!_levels.empty())
    {
        _weathers[_currentLevelIndex].Update(deltaTime);
        _scrollingClouds[_currentLevelIndex].Update(deltaTime);
        _props[_currentLevelIndex].Update(deltaTime);
        _hiddenWalls[_currentLevelIndex].Update(deltaTime, _player->GetColliderBounds());
        _player->SetWindForceX(CurrentLevel().hasWind
            ? _wind.GetForce() * WIND_FORCE_ACCEL
            : 0.0f);
    }

    Scene::Update(deltaTime);

    Vector2 jumpEffectPosition;
    PlatformMaterial jumpEffectMaterial;
    if (_player->ConsumeJumpEffectTrigger(jumpEffectPosition, jumpEffectMaterial))
    {
        _jumpEffect.Play(jumpEffectPosition, jumpEffectMaterial);
    }
    _jumpEffect.Update(deltaTime);

    CheckLevelTransition(deltaTime);
}

void GameScene::CheckLevelTransition(float deltaTime)
{
    // player_king.png 셀(36x36) 안에서 콜라이더가 offsetY(14px)만큼 아래로
    // 밀려 있어서, position(스프라이트 앵커)은 콜라이더보다 위쪽에 있다. 화면
    // 경계 판정을 anchor 기준으로 하면 화면 위쪽 36px 안의 낮은 플랫폼 위에
    // 가만히 서 있기만 해도 anchor가 음수가 되어 매 프레임 전환이 발생한다.
    //
    // 위/아래 판정 모두 콜라이더의 "위쪽" 한 점만 기준으로 삼는다. 위쪽은
    // Bottom(), 아래쪽은 Top()처럼 서로 다른 기준점을 섞으면, 콜라이더 높이(22px)만큼
    // 좌표가 어긋나서 전환 직후 새 레벨에서 반대 방향 경계를 즉시 다시 넘은 것으로
    // 계산되어 버린다(도착하자마자 되튕겨나가 반복 전환됨).
    const Rect colliderBounds = _player->GetColliderBounds();

    
       

    if (colliderBounds.Top() < 0.0f && _currentLevelIndex +1 < static_cast<int>(_levels.size()))
    {
        float nextY = _player->GetPosition().y + SCREEN_HEIGHT;
        TransitionToLevel(_currentLevelIndex + 1, nextY, deltaTime);
    }
    else if (colliderBounds.Top() > SCREEN_HEIGHT)
    {
        float nextY = _player->GetPosition().y - SCREEN_HEIGHT;
        TransitionToLevel(_currentLevelIndex - 1, nextY, deltaTime);
    }    
}

void GameScene::TransitionToLevel(int newIndex, float newY, float deltaTime)
{
    // 텍스처/날씨는 Init에서 레벨별로 미리 로드해 두었으므로 여기서는 인덱스만 바꾼다
    // (매 전환마다 디스크에서 다시 로드하면 그 프레임이 느려져 다음 프레임 충돌이 씹힘).
    _currentLevelIndex = newIndex;

    // 충돌 검사가 새 레벨의 플랫폼을 봐야 하므로 위치를 보정하기 전에 먼저 교체한다.
    _player->SetPlatforms(&CurrentLevel().platforms);

    Vector2 position = _player->GetPosition();
    position.y = newY;

    // 텔레포트된 위치를 그 프레임 안에 새 플랫폼 기준으로 검증한다.
    // 그렇지 않으면 전환 지점 바로 근처 플랫폼과의 충돌이 한 프레임 씹힌다.
    _player->HorizontalCollision(position);
    _player->VerticalCollision(position, deltaTime, false);

    _player->SetPosition(position);
}

void GameScene::Render(const RenderContext& context)
{
    // 먼저 그린 이미지가 뒤에 놓이고, 나중에 그린 이미지가 앞을 덮습니다.
    if (!_levels.empty())
    {
        RenderTextureLayer(_backgroundTextures[_currentLevelIndex], _isBackgroundLoaded[_currentLevelIndex], context);
        _scrollingClouds[_currentLevelIndex].Render(context, ScrollLayer::Background);
        RenderTextureLayer(_midgroundTextures[_currentLevelIndex], _isMidgroundLoaded[_currentLevelIndex], context);
        // 소품은 미드그라운드보다 앞, 플레이어보다는 뒤에 깔립니다.
        _props[_currentLevelIndex].Render(context);
    }

    Scene::Render(context);
    _jumpEffect.Render(context);

    if (!_levels.empty())
    {
        _weathers[_currentLevelIndex].Render(context, CurrentLevel().hasWind, _wind.GetScrollOffset());
        // Foreground는 플레이어보다 앞에 보여야 하므로 Actor 렌더링 뒤에 그립니다.
        RenderTextureLayer(_foregroundTextures[_currentLevelIndex], _isForegroundLoaded[_currentLevelIndex], context);
        // hidden wall은 foreground보다도 앞, 플레이어를 가려서 "진짜 벽" 착시를 줘야 합니다.
        _hiddenWalls[_currentLevelIndex].Render(context);
        // fg 구름/안개는 포그라운드보다도 위, 플레이어를 가릴 수 있는 맨 앞 레이어입니다.
        _scrollingClouds[_currentLevelIndex].Render(context, ScrollLayer::Foreground);
    }
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

