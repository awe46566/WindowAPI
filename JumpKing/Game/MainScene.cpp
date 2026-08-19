#include "pch.h"
#include "framework.h"
#include "Engine/ResourceCatalog.h"
#include "Engine/InputManager.h"
#include "Engine/GameConstants.h"
#include "Engine/RenderContext.h"
#include "Engine/Game.h"
#include "FrameWork/SceneManager.h"
#include "Game/MainScene.h"


void MainScene::Init()
{   
    const ImageResource* image = ResourceCatalog::GetInstance().FindImage("lobby_logo");
    if (image != nullptr)
    {
        _isLogoLoaded = _logoTexture.Load(image->path, image->rows, image->columns);
    }

    wchar_t executablePath[MAX_PATH]{};
    const DWORD pathLength = GetModuleFileNameW(
        nullptr,
        executablePath,
        ARRAYSIZE(executablePath));

    if (pathLength > 0 && pathLength < ARRAYSIZE(executablePath))
    {
        const fs::path fontPath =
            fs::path(executablePath).parent_path() / L".." / L".." / L"Resource" / L"Fonts" / L"ttf_alkhemikal.ttf";

        AddFontResourceExW(fontPath.c_str(), FR_PRIVATE, nullptr);
        Game::GetInstance().GetWriteFactory()->CreateTextFormat(
            L"ttf_alkhemikal.ttf",                        // fontFamilyName — ttf_alkhemikal의 실제 family name
            nullptr,                       // fontCollection — 시스템 컬렉션 사용
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            24.0f,                         // fontSize — 원하는 크기로 조절
            L"en-us",                      // localeName
            _titleTextFormat.GetAddressOf());
    }
}

void MainScene::Update(float deltaTime)
{  
    if (InputManager::GetInstance().GetButtonDown(KeyType::Space))
    {
        SceneManager::GetInstance().RequestSceneChange(SceneType::Game);
    }
}

void MainScene::Render(const RenderContext& context)
{
    // TODO: 로고 텍스처 렌더링 + "PRESS SPACE TO START" 텍스트 렌더링
    HRESULT DrawText(
        const WCHAR * string,
        UINT32 stringLength,
        IDWriteTextFormat * textFormat,
        const D2D1_RECT_F & layoutRect,
        ID2D1Brush * defaultForegroundBrush);

    const D2D1_RECT_F sourceRect = D2D1::RectF(
        0.0f,
        0.0f,
        static_cast<float>(_logoTexture.GetWidth()),
        static_cast<float>(_logoTexture.GetHeight()));

    const D2D1_RECT_F BackGroundRect = D2D1::RectF(
        0.0f,
        0.0f,
        static_cast<float>(GameConstants::SCREEN_WIDTH),
        static_cast<float>(GameConstants::SCREEN_HEIGHT));

    const D2D1_RECT_F TextRect = D2D1::RectF(
        0.0f,
        0.0f,
        static_cast<float>(GameConstants::SCREEN_WIDTH),
        static_cast<float>(GameConstants::SCREEN_HEIGHT));

    context.target->FillRectangle(BackGroundRect, context.defaultBrush);

    _logoTexture.Render(
        context,
        Vector2{GameConstants::SCREEN_WIDTH / 10, GameConstants::SCREEN_WIDTH / 12 },
        sourceRect);

    const std::wstring StartText = L"PRESS SPACE TO START";

    const D2D1_COLOR_F previousColor = context.defaultBrush->GetColor();
    context.defaultBrush->SetColor(D2D1::ColorF(D2D1::ColorF::White));

    context.target->DrawText(
        StartText.c_str(),
        static_cast<UINT32>(StartText.length()),
        _titleTextFormat.Get(),
        TextRect,
        context.defaultBrush);

    context.defaultBrush->SetColor(previousColor);
}
