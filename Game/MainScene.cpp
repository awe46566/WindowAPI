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
            fs::path(executablePath).parent_path() / L".." / L".." / L"Resource" / L"Fonts" / L"ttf_pixolde_bold.ttf";

        Microsoft::WRL::ComPtr<IDWriteFactory5> factory5;
        Game::GetInstance().GetWriteFactory()->QueryInterface(IID_PPV_ARGS(&factory5));

        Microsoft::WRL::ComPtr<IDWriteFontSetBuilder1> fontSetBuilder;
        factory5->CreateFontSetBuilder(&fontSetBuilder);

        Microsoft::WRL::ComPtr<IDWriteFontFile> fontFile;
        factory5->CreateFontFileReference(fontPath.c_str(), nullptr, &fontFile);
        fontSetBuilder->AddFontFile(fontFile.Get());

        Microsoft::WRL::ComPtr<IDWriteFontSet> fontSet;
        fontSetBuilder->CreateFontSet(&fontSet);

        factory5->CreateFontCollectionFromFontSet(fontSet.Get(), _fontCollection.GetAddressOf());


        Game::GetInstance().GetWriteFactory()->CreateTextFormat(
            L"Pixolde", 
            _fontCollection.Get(),
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            16.0f,                         // fontSize — 원하는 크기로 조절
            L"en-us",              
            _titleTextFormat.GetAddressOf());

        // 가로/세로 중앙 정렬 옵션
        _titleTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        _titleTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    }
}

void MainScene::Update(float deltaTime)
{  
    if (InputManager::GetInstance().GetButtonDown(KeyType::Space))
    {
        SceneManager::GetInstance().RequestSceneChange(SceneType::Game);
    }

    _blinkTimer += deltaTime;
    if (_blinkTimer >= 0.5f)
    {
        _blinkTimer = 0.0f;
        _isTextVisible = !_isTextVisible;
    }
}

void MainScene::Render(const RenderContext& context)
{
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
        static_cast<float>(GameConstants::SCREEN_HEIGHT + 180));

    context.target->FillRectangle(BackGroundRect, context.defaultBrush);

    _logoTexture.Render(
        context,
        Vector2{GameConstants::SCREEN_WIDTH / 10, GameConstants::SCREEN_WIDTH / 12 },
        sourceRect);

    const std::wstring StartText = L"PRESS SPACE";

    const D2D1_COLOR_F previousColor = context.defaultBrush->GetColor();
    context.defaultBrush->SetColor(D2D1::ColorF(D2D1::ColorF::White));

    if (_isTextVisible)
    {
        context.target->DrawText(
            StartText.c_str(),
            static_cast<UINT32>(StartText.length()),
            _titleTextFormat.Get(),
            TextRect,
            context.defaultBrush);
    }

    context.defaultBrush->SetColor(previousColor);
}
