#include "pch.h"
#include "framework.h"
#include "Engine/RenderContext.h"
#include "Game/GameScene.h"
#include "Player.h"

void GameScene::Init()
{
    wchar_t executablePath[MAX_PATH]{};
    const DWORD pathLength = GetModuleFileNameW(
        nullptr,
        executablePath,
        ARRAYSIZE(executablePath));

    if (pathLength > 0 && pathLength < ARRAYSIZE(executablePath))
    {
        const fs::path resourcePath =
            fs::path(executablePath).parent_path()
            / L".."
            / L".."
            / L"Resource"
            / L"MG"
            / L"1.png";

        _isBackgroundLoaded =
            _backgroundTexture.Load(resourcePath.lexically_normal(), 1, 1);
    }

    Player* player = new Player();
    player->SetPosition(Vector2{ 230.0f, 286.0f });
    AddActor(player);
}

void GameScene::Render(const RenderContext& context)
{
    if (_isBackgroundLoaded)
    {
        // 현재는 이미지 전체를 sourceRect로 선택합니다.
        // 스프라이트 애니메이션에서는 이 사각형을 프레임 크기만큼 이동시킵니다.
        const D2D1_RECT_F sourceRect = D2D1::RectF(
            0.0f,
            0.0f,
            static_cast<float>(_backgroundTexture.GetWidth()),
            static_cast<float>(_backgroundTexture.GetHeight()));

        _backgroundTexture.Render(context, Vector2{}, sourceRect);
    }

    Scene::Render(context);
    
}
