#pragma once

#include <cstdint>

struct ID2D1HwndRenderTarget;
struct ID2D1SolidColorBrush;
struct IDWriteTextFormat;

// 한 프레임 동안 Scene과 Component가 그릴 때 필요한 Direct2D 자원 묶음입니다.
// 실제 소유자는 Game이며, RenderContext는 포인터를 잠시 빌려 전달하기만 합니다.
// 따라서 RenderContext나 내부 포인터를 프레임이 끝난 뒤 따로 저장하면 안 됩니다.
struct RenderContext
{
    ID2D1HwndRenderTarget* target = nullptr;
    ID2D1SolidColorBrush* defaultBrush = nullptr;
    IDWriteTextFormat* defaultTextFormat = nullptr;

    // RenderTarget이 재생성될 때 증가합니다.
    // Texture는 이 값을 보고 ID2D1Bitmap을 다시 만들어야 하는지 판단합니다.
    std::uint64_t deviceGeneration = 0;
};
