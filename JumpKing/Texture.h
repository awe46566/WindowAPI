#pragma once

#include "Engine/Types.h"
#include <d2d1.h>
#include <wincodec.h>
#include <wrl/client.h>
#include <string>

struct RenderContext;

class Texture
{
public:
    bool Load(
        const std::wstring& path,
        int32 row = 1,
        int32 col = 1);

    void Render(
        const RenderContext& context,
        const Vector2& position,
        const D2D1_RECT_F& sourceRect);

    int32 GetWidth() const { return _width; }
    int32 GetHeight() const { return _height; }
    D2D1_SIZE_F GetFrameSize() const
    {
        return D2D1::SizeF(
            static_cast<float>(_frameWidth),
            static_cast<float>(_frameHeight));
    }

private:
    bool CreateBitmap(const RenderContext& context);

    // WIC 이미지는 장치와 독립적이므로 RenderTarget이 재생성되어도 유지합니다.
    Microsoft::WRL::ComPtr<IWICFormatConverter> _wicSource;

    // ID2D1Bitmap은 특정 RenderTarget에 속하는 장치 의존 자원입니다.
    Microsoft::WRL::ComPtr<ID2D1Bitmap> _bitmap;
    uint64 _bitmapGeneration = 0;

    // 전체 이미지 크기와 스프라이트 시트 분할 정보입니다.
    int32 _width = 0;
    int32 _height = 0;
    int32 _row = 1;
    int32 _col = 1;
    int32 _frameWidth = 0;
    int32 _frameHeight = 0;
};
