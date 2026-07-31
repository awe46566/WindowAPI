#include "pch.h"
#include "framework.h"
#include "Engine/Game.h"
#include "Engine/InputManager.h"
#include "Engine/RenderContext.h"
#include "Engine/TimeManager.h"
#include "Framework/SceneManager.h"

#include <string>

bool Game::Init(HWND window)
{
    _window = window;

    // Factory는 Direct2D 자원을 만들어 주는 출발점입니다.
    // SINGLE_THREADED는 렌더링 호출을 현재 게임 스레드 하나에서 수행한다는 뜻입니다.
    HRESULT result = D2D1CreateFactory(
        D2D1_FACTORY_TYPE_SINGLE_THREADED,
        _d2dFactory.GetAddressOf());
    if (FAILED(result))
    {
        Cleanup();
        return false;
    }

    // Direct2D는 글꼴 배치 기능을 직접 제공하지 않으므로
    // 텍스트 출력에는 별도의 DirectWrite Factory를 사용합니다.
    result = DWriteCreateFactory(
        DWRITE_FACTORY_TYPE_SHARED,
        __uuidof(IDWriteFactory),
        reinterpret_cast<IUnknown**>(_dwriteFactory.GetAddressOf()));
    if (FAILED(result))
    {
        Cleanup();
        return false;
    }

    // WIC(Windows Imaging Component)는 PNG 같은 파일을 픽셀 데이터로 디코딩합니다.
    // CoCreateInstance를 호출하려면 wWinMain에서 COM이 먼저 초기화되어 있어야 합니다.
    result = CoCreateInstance(
        CLSID_WICImagingFactory,
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(_wicFactory.GetAddressOf()));
    if (FAILED(result))
    {
        Cleanup();
        return false;
    }

    // TextFormat은 글꼴, 크기, 스타일을 묶은 재사용 가능한 텍스트 설정입니다.
    result = _dwriteFactory->CreateTextFormat(
        L"Segoe UI",
        nullptr,
        DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        18.0f,
        L"ko-kr",
        _defaultTextFormat.GetAddressOf());
    if (FAILED(result) || !CreateDeviceResources())
    {
        Cleanup();
        return false;
    }

    TimeManager::GetInstance().Init();
    InputManager::GetInstance().Init(_window);
    SceneManager::GetInstance().Init();

    return true;
}

bool Game::CreateDeviceResources()
{
    // 이미 자원이 있다면 프레임마다 다시 만들지 않습니다.
    // Direct2D 자원 생성은 그리기 호출보다 비용이 크므로 가능한 한 재사용해야 합니다.
    if (_renderTarget != nullptr)
    {
        return true;
    }

    RECT clientRect{};
    GetClientRect(_window, &clientRect);

    const D2D1_SIZE_U renderSize = D2D1::SizeU(
        static_cast<UINT32>(clientRect.right - clientRect.left),
        static_cast<UINT32>(clientRect.bottom - clientRect.top));

    // HwndRenderTarget은 HWND와 연결된 실제 그리기 대상입니다.
    // 기존 GDI 방식의 메모리 DC와 BitBlt 역할을 Direct2D가 내부적으로 처리합니다.
    HRESULT result = _d2dFactory->CreateHwndRenderTarget(
        D2D1::RenderTargetProperties(),
        D2D1::HwndRenderTargetProperties(_window, renderSize),
        _renderTarget.GetAddressOf());
    if (FAILED(result))
    {
        return false;
    }

    // Brush와 Bitmap은 RenderTarget에서 생성되는 '장치 의존 자원'입니다.
    // RenderTarget이 사라지면 여기서 만든 Brush도 함께 다시 만들어야 합니다.
    result = _renderTarget->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::Black),
        _defaultBrush.GetAddressOf());
    if (FAILED(result))
    {
        DiscardDeviceResources();
        return false;
    }

    // Texture가 자신이 가진 ID2D1Bitmap이 현재 RenderTarget용인지 판별할 때 사용합니다.
    ++_deviceGeneration;
    return true;
}

void Game::DiscardDeviceResources()
{
    // ComPtr::Reset()은 COM 객체의 Release를 자동 호출합니다.
    // 생성 순서의 반대로 종속 자원인 Brush를 먼저 해제합니다.
    _defaultBrush.Reset();
    _renderTarget.Reset();
}

void Game::Cleanup()
{
    SceneManager::GetInstance().Cleanup();

    DiscardDeviceResources();
    _defaultTextFormat.Reset();
    _wicFactory.Reset();
    _dwriteFactory.Reset();
    _d2dFactory.Reset();
    _window = nullptr;
}

void Game::Update()
{
    TimeManager::GetInstance().Update();
    InputManager::GetInstance().Update();
    SceneManager::GetInstance().Update(
        TimeManager::GetInstance().GetDeltaTime());
}

void Game::Render()
{
    if (!CreateDeviceResources())
    {
        return;
    }

    // Direct2D의 한 프레임은 반드시 BeginDraw와 EndDraw 사이에 기록합니다.
    // DrawText, DrawBitmap 같은 호출은 이 구간 안에서만 수행합니다.
    _renderTarget->BeginDraw();

    // 이전 Actor가 남긴 이동·회전 변환이 다음 프레임에 영향을 주지 않도록 초기화합니다.
    _renderTarget->SetTransform(
        D2D1::Matrix3x2F::Scale(
            static_cast<float>(WINDOW_SCALE), 
            static_cast<float>(WINDOW_SCALE)
        )
    );
    _renderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

    // Scene은 자원을 소유하지 않고, 이 프레임에 사용할 포인터만 빌려 받습니다.
    const RenderContext context
    {
        _renderTarget.Get(),
        _defaultBrush.Get(),
        _defaultTextFormat.Get(),
        _deviceGeneration
    };

    SceneManager::GetInstance().Render(context);

    const std::wstring fpsText =
        L"FPS: " + std::to_wstring(TimeManager::GetInstance().GetFPS());
    const D2D1_RECT_F fpsRect = D2D1::RectF(20.0f, 20.0f, 220.0f, 50.0f);
    _renderTarget->DrawTextW(
        fpsText.c_str(),
        static_cast<UINT32>(fpsText.size()),
        _defaultTextFormat.Get(),
        fpsRect,
        _defaultBrush.Get());

    // 실제 그리기 결과와 장치 상태는 EndDraw에서 확인합니다.
    const HRESULT result = _renderTarget->EndDraw();
    if (result == D2DERR_RECREATE_TARGET)
    {
        // 창이나 그래픽 장치 상태가 바뀌면 RenderTarget이 무효가 될 수 있습니다.
        // 여기서는 장치 의존 자원만 버리고 다음 Render에서 다시 생성합니다.
        DiscardDeviceResources();
    }
}

void Game::Resize(uint32 width, uint32 height)
{
    // 최소화되면 width 또는 height가 0이 될 수 있으므로 Resize하지 않습니다.
    if (_renderTarget != nullptr && width > 0 && height > 0)
    {
        const HRESULT result = _renderTarget->Resize(D2D1::SizeU(width, height));
        if (FAILED(result))
        {
            DiscardDeviceResources();
        }
    }
}
