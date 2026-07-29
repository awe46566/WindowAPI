#include "pch.h"
#include "framework.h"
#include "Engine/Game.h"
#include "Engine/InputManager.h"
#include "Engine/TimeManager.h"
#include "Framework/SceneManager.h"

#include <string>

bool Game::Init(HWND window)
{
    _window = window;
    _windowDC = GetDC(_window);
    if (_windowDC == nullptr)
    {
        return false;
    }

    GetClientRect(_window, &_clientRect);

    _backBufferDC = CreateCompatibleDC(_windowDC);
    _backBufferBitmap = CreateCompatibleBitmap(
        _windowDC,
        _clientRect.right,
        _clientRect.bottom);

    if (_backBufferDC == nullptr || _backBufferBitmap == nullptr)
    {
        Cleanup();
        return false;
    }

    _previousBitmap = static_cast<HBITMAP>(
        SelectObject(_backBufferDC, _backBufferBitmap));

    TimeManager::GetInstance().Init();
    InputManager::GetInstance().Init(_window);
    SceneManager::GetInstance().Init();

    return true;
}

void Game::Cleanup()
{
    SceneManager::GetInstance().Cleanup();

    if (_backBufferDC != nullptr && _previousBitmap != nullptr)
    {
        SelectObject(_backBufferDC, _previousBitmap);
        _previousBitmap = nullptr;
    }

    if (_backBufferBitmap != nullptr)
    {
        DeleteObject(_backBufferBitmap);
        _backBufferBitmap = nullptr;
    }

    if (_backBufferDC != nullptr)
    {
        DeleteDC(_backBufferDC);
        _backBufferDC = nullptr;
    }

    if (_windowDC != nullptr && _window != nullptr)
    {
        ReleaseDC(_window, _windowDC);
        _windowDC = nullptr;
    }

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
    PatBlt(
        _backBufferDC,
        0,
        0,
        _clientRect.right,
        _clientRect.bottom,
        WHITENESS);

    SceneManager::GetInstance().Render(_backBufferDC);

    const std::wstring fpsText =
        L"FPS: " + std::to_wstring(TimeManager::GetInstance().GetFPS());
    TextOutW(
        _backBufferDC,
        20,
        20,
        fpsText.c_str(),
        static_cast<int>(fpsText.size()));

    BitBlt(
        _windowDC,
        0,
        0,
        _clientRect.right,
        _clientRect.bottom,
        _backBufferDC,
        0,
        0,
        SRCCOPY);
}
