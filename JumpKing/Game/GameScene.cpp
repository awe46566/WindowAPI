#include "pch.h"
#include "framework.h"
#include "Game/GameScene.h"

void GameScene::Init()
{
    // 다음 단계에서 Player와 Block을 생성할 자리입니다.
}

void GameScene::Render(HDC hdc)
{
    Scene::Render(hdc);

    constexpr wchar_t MESSAGE[] = L"JumpKing framework is running";
    TextOutW(hdc, 20, 50, MESSAGE, ARRAYSIZE(MESSAGE) - 1);
}
