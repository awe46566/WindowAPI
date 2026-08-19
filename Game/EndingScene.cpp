#include "pch.h"
#include "framework.h"
#include "Engine/Game.h"
#include "Engine/GameConstants.h"
#include "Engine/InputManager.h"
#include "Engine/RenderContext.h"
#include "Framework/SceneManager.h"
#include "Game/EndingScene.h"

void EndingScene::Init()
{
    wchar_t executablePath[MAX_PATH]{};
    const DWORD pathLength = GetModuleFileNameW(
        nullptr,
        executablePath,
        ARRAYSIZE(executablePath));

    if (pathLength > 0 && pathLength < ARRAYSIZE(executablePath))
    {
        const fs::path fontPath =
            fs::path(executablePath).parent_path() / L"Resource" / L"Fonts" / L"ttf_pixolde_bold.ttf";

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
            30.0f,                         // fontSize — 원하는 크기로 조절
            L"en-us",
            _titleTextFormat.GetAddressOf());

        // 가로/세로 중앙 정렬 옵션
        _titleTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        _titleTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    }
}

void EndingScene::Update(float deltaTime)
{
    // TODO: InputManager::GetInstance().GetButtonDown(KeyType::Space) 확인 후
    // SceneManager::GetInstance().RequestSceneChange(SceneType::Main)
    if (InputManager::GetInstance().GetButtonDown(KeyType::Space))
    {
        SceneManager::GetInstance().RequestSceneChange(SceneType::Main);
    }
}

void EndingScene::Render(const RenderContext& context)
{
    // TODO: "THE END" + "PRESS SPACE TO RETURN" 텍스트 렌더링
    const D2D1_RECT_F BackGroundRect = D2D1::RectF(
        0.0f,
        0.0f,
        static_cast<float>(GameConstants::SCREEN_WIDTH),
        static_cast<float>(GameConstants::SCREEN_HEIGHT));

    context.target->FillRectangle(BackGroundRect, context.defaultBrush);

    const D2D1_RECT_F TextRect = D2D1::RectF(
        0.0f,
        0.0f,
        static_cast<float>(GameConstants::SCREEN_WIDTH),
        static_cast<float>(GameConstants::SCREEN_HEIGHT));

    const std::wstring EndText = L"THE END";

    const D2D1_COLOR_F previousColor = context.defaultBrush->GetColor();
    context.defaultBrush->SetColor(D2D1::ColorF(D2D1::ColorF::White));

    
    context.target->DrawText(
        EndText.c_str(),
        static_cast<UINT32>(EndText.length()),
        _titleTextFormat.Get(),
        TextRect,
        context.defaultBrush);
   
    context.defaultBrush->SetColor(previousColor);
}
