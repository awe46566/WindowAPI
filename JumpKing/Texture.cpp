#include "pch.h"
#include "Engine/Game.h"
#include "Engine/RenderContext.h"
#include "Texture.h"

bool Texture::Load(const wstring& path, int32 row, int32 col)
{
    if (path.empty() || row <= 0 || col <= 0)
    {
        return false;
    }

    IWICImagingFactory* wicFactory = Game::GetInstance().GetWicFactory();
    if (wicFactory == nullptr)
    {
        return false;
    }

    // 1단계: 파일 형식을 읽을 수 있는 Decoder를 만듭니다.
    // 이 단계에서는 아직 Direct2D Bitmap이 만들어지지 않습니다.
    Microsoft::WRL::ComPtr<IWICBitmapDecoder> decoder;
    HRESULT result = wicFactory->CreateDecoderFromFilename(
        path.c_str(),                   //c_str은 wchar_t* 를 가르킴(이미지 파일의 경로)
        nullptr,      
        GENERIC_READ,                   //파일을 읽기 권한으로 열음
        WICDecodeMetadataCacheOnLoad,   //메타데이터를 언제 읽을 것인지 지정 ( 메타데이터 = 이미지 크기, 색상 프로필, 방향 정보 등등)
        decoder.GetAddressOf());
    if (FAILED(result))
    {
        return false;
    }

    // PNG도 여러 프레임을 가질 수 있으므로 첫 번째 프레임을 명시적으로 선택합니다.
    Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> frame;
    result = decoder->GetFrame(0, frame.GetAddressOf());
    if (FAILED(result))
    {
        return false;
    }

    // 2단계: Direct2D가 알파 합성하기 좋은 픽셀 형식으로 변환합니다.
    // -> WIC가 읽은 원본 이미지의 픽셀 형식이 다양하기 때문에 Converter로 하나의 통일된 형식으로 바꿈.
    Microsoft::WRL::ComPtr<IWICFormatConverter> converter;
    result = wicFactory->CreateFormatConverter(converter.GetAddressOf());
    if (FAILED(result))
    {
        return false;
    }

    result = converter->Initialize(
        frame.Get(),
        // PBGRA의 P는 Premultiplied Alpha입니다.
        // RGB에 Alpha가 미리 곱해진 형식이며 Direct2D의 기본 알파 합성 형식입니다.
        GUID_WICPixelFormat32bppPBGRA,
        WICBitmapDitherTypeNone,
        nullptr,
        0.0,
        WICBitmapPaletteTypeMedianCut);
    if (FAILED(result))
    {
        return false;
    }

    UINT width = 0;
    UINT height = 0;
    result = converter->GetSize(&width, &height);
    if (FAILED(result) || width == 0 || height == 0)
    {
        return false;
    }

    // 스프라이트 시트를 행과 열로 정확하게 나눌 수 있어야
    // 프레임마다 픽셀이 어긋나지 않습니다.
    if (width % static_cast<UINT>(col) != 0 || height % static_cast<UINT>(row) != 0)
    {
        return false;
    }

    // WIC Source는 CPU 쪽의 장치 독립 데이터입니다.
    // Decoder와 Frame은 지역 ComPtr이므로 함수 종료 시 자동 해제됩니다.
    _wicSource = move(converter);
    _bitmap.Reset();
    _bitmapGeneration = 0;

    _width = static_cast<int32>(width);
    _height = static_cast<int32>(height);
    _row = row;
    _col = col;
    _frameWidth = _width / _col;
    _frameHeight = _height / _row;

    return true;
}

bool Texture::CreateBitmap(const RenderContext& context)
{
    if (_wicSource == nullptr || context.target == nullptr)
    {
        return false;
    }

    // 현재 RenderTarget에서 만든 Bitmap이라면 그대로 재사용합니다.
    if (_bitmap != nullptr && _bitmapGeneration == context.deviceGeneration)
    {
        return true;
    }

    _bitmap.Reset();
    _bitmapGeneration = 0;

    // 3단계: WIC 픽셀 데이터를 현재 RenderTarget이 그릴 수 있는
    // 장치 의존 ID2D1Bitmap으로 변환합니다.
    const HRESULT result = context.target->CreateBitmapFromWicBitmap(
        _wicSource.Get(),
        nullptr,
        _bitmap.GetAddressOf());
    if (FAILED(result))
    {
        return false;
    }

    _bitmapGeneration = context.deviceGeneration;
    return true;
}

void Texture::Render(const RenderContext& context, const Vector2& position, const D2D1_RECT_F& sourceRect)
{
    if (!CreateBitmap(context))
    {
        return;
    }

    const float sourceWidth = sourceRect.right - sourceRect.left;
    const float sourceHeight = sourceRect.bottom - sourceRect.top;
    // sourceRect는 원본 이미지에서 자를 영역이고,
    // destinationRect는 화면에서 그 영역이 놓일 위치와 크기입니다.
    const D2D1_RECT_F destinationRect = D2D1::RectF(
        position.x,
        position.y,
        position.x + sourceWidth,
        position.y + sourceHeight);

    context.target->DrawBitmap(
        _bitmap.Get(),
        destinationRect,
        1.0f,        
        D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, // 픽셀 아트 확대 시 색을 섞지 않아 경계가 흐려지는 것을 막습니다.
        sourceRect);
}
