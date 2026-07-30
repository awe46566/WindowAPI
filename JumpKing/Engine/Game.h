#pragma once

#include "Engine/Singleton.h"
#include "Engine/Types.h"
#include <wrl/client.h>

struct ID2D1Factory;
struct ID2D1HwndRenderTarget;
struct ID2D1SolidColorBrush;
struct IDWriteFactory;
struct IDWriteTextFormat;
struct IWICImagingFactory;

class Game : public Singleton<Game>
{
    friend class Singleton<Game>;

public:
    bool Init(HWND window);
    void Cleanup();
    void Update();
    void Render();
    void Resize(uint32 width, uint32 height);

    ID2D1HwndRenderTarget* GetRenderTarget() const
    {
        return _renderTarget.Get();
    }

    IWICImagingFactory* GetWicFactory() const
    {
        return _wicFactory.Get();
    }

private:
    Game() = default;
    ~Game() = default;

    bool CreateDeviceResources();
    void DiscardDeviceResources();

    HWND _window = nullptr;

    // ComPtr은 COM 참조 횟수를 관리해 직접 Release할 필요가 없게 해 줍니다.
    // Factory/WIC/DirectWrite는 장치 독립 자원이라 게임 수명 동안 유지합니다.
    Microsoft::WRL::ComPtr<ID2D1Factory> _d2dFactory;
    Microsoft::WRL::ComPtr<ID2D1HwndRenderTarget> _renderTarget;
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> _defaultBrush;
    Microsoft::WRL::ComPtr<IDWriteFactory> _dwriteFactory;
    Microsoft::WRL::ComPtr<IDWriteTextFormat> _defaultTextFormat;
    Microsoft::WRL::ComPtr<IWICImagingFactory> _wicFactory;
    uint64 _deviceGeneration = 0;
};
