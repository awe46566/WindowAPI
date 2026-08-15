#include "pch.h"
#include "Weather.h"
#include "Engine/ResourceCatalog.h"
#include "Engine/RenderContext.h"
#include "Engine/GameConstants.h"
using namespace GameConstants;

namespace
{
    const char* WeatherVariantName(WeatherType type)
    {
        switch (type)
        {
            case WeatherType::Rain: return "rain";
            case WeatherType::LightRain: return "light_rain";
            case WeatherType::Snow: return "snow";
            case WeatherType::LightSnow: return "light_snow";
            default: return nullptr;
        }
    }
}

bool Weather::LoadVariant(WeatherType type, int levelIndex)
{
    _isLoaded = false;
    _hasMask = false;
    _currentFrame = 0;
    _frameTimer = 0.0f;

    const char* name = WeatherVariantName(type);
    if (name == nullptr)
    {
        return false;
    }

    for (int i = 0; i < 4; ++i)
    {
        const string imageId = string("weather_") + name + "_" + to_string(i);
        const ImageResource* image = ResourceCatalog::GetInstance().FindImage(imageId);

        if (image == nullptr || !_frames[i].Load(image->path, image->rows, image->columns))
        {
            return false;
        }
    }

    // 마스크는 날씨 종류가 아니라 레벨 전용 에셋이라, 등록 안 된 레벨이면
    // 그냥 마스크 없이(화면 전체) 그린다 - 실패해도 치명적이지 않음.
    const string maskId = string("weathermask_") + to_string(levelIndex);
    const ImageResource* maskImage = ResourceCatalog::GetInstance().FindImage(maskId);
    _hasMask = maskImage != nullptr && _mask.Load(maskImage->path, maskImage->rows, maskImage->columns);

    _isLoaded = true;
    return true;
}

void Weather::Update(float deltaTime)
{
    // TODO(user): Engine/SpriteRenderer.cpp의 SpriteRenderer::Update 누적/소진(catch-up)
    // while 루프를 참고해 채운다.
    //   - _isLoaded가 false면 아무것도 하지 않는다.
    //   - _frameTimer += deltaTime.
    //   - _frameTimer가 WEATHER_FRAME_INTERVAL 이상인 동안(while) 반복해서
    //     _frameTimer -= WEATHER_FRAME_INTERVAL, _currentFrame = (_currentFrame + 1) % 4.
    if (!_isLoaded)
        return;

    _frameTimer += deltaTime;

    while (_frameTimer >= WEATHER_FRAME_INTERVAL)
    {
        _frameTimer -= WEATHER_FRAME_INTERVAL;
        _currentFrame = (_currentFrame + 1) % 4;
    }
}

void Weather::Render(const RenderContext& context, bool hasWind, float windScrollOffset)
{
    if (!_isLoaded)
        return;

    Texture& texture = _frames[_currentFrame];
    const D2D1_RECT_F sourceRect = D2D1::RectF(
        0.0f,
        0.0f,
        static_cast<float>(texture.GetWidth()),
        static_cast<float>(texture.GetHeight()));

    float width = static_cast<float>(texture.GetWidth());
    float height = static_cast<float>(texture.GetHeight());
    float baseX = hasWind ? fmodf(windScrollOffset * 60, width) : 0.0f;

    // 마스크가 있으면 지형 실루엣 알파를 오파시티 마스크로 써서, 그 레이어
    // 안에서 그리는 것만 마스크 알파만큼 곱해져서 보이게 한다(지형 위에만 비/눈이 겹쳐 보임).
    Microsoft::WRL::ComPtr<ID2D1BitmapBrush> maskBrush;
    Microsoft::WRL::ComPtr<ID2D1Layer> layer;
    bool layerPushed = false;

    if (_hasMask)
    {
        ID2D1Bitmap* maskBitmap = _mask.GetOrCreateBitmap(context);
        if (maskBitmap != nullptr &&
            SUCCEEDED(context.target->CreateBitmapBrush(maskBitmap, maskBrush.GetAddressOf())) &&
            SUCCEEDED(context.target->CreateLayer(nullptr, layer.GetAddressOf())))
        {
            context.target->PushLayer(
                D2D1::LayerParameters(
                    D2D1::InfiniteRect(),
                    nullptr,
                    D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
                    D2D1::IdentityMatrix(),
                    1.0f,
                    maskBrush.Get()),
                layer.Get());
            layerPushed = true;
        }
    }

    for (float y = 0.0f; y < SCREEN_HEIGHT; y += height)
    {
        for (float x = baseX - width; x < SCREEN_WIDTH; x += width)
        {
            texture.Render(context, Vector2{ x, y }, sourceRect);
        }
    }

    if (layerPushed)
    {
        context.target->PopLayer();
    }
}
