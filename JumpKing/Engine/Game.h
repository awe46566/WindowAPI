#pragma once

#include "Engine/Singleton.h"

class Game : public Singleton<Game>
{
    friend class Singleton<Game>;

public:
    bool Init(HWND window);
    void Cleanup();
    void Update();
    void Render();

private:
    Game() = default;
    ~Game() = default;

    HWND _window = nullptr;
    RECT _clientRect{};
    HDC _windowDC = nullptr;
    HDC _backBufferDC = nullptr;
    HBITMAP _backBufferBitmap = nullptr;
    HBITMAP _previousBitmap = nullptr;
};
