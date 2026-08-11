#pragma once

#include "Engine/Singleton.h"
#include "Engine/Types.h"

#include <array>

enum class KeyType : uint32
{
    Left = VK_LEFT,
    Right = VK_RIGHT,
    Up = VK_UP,
    Down = VK_DOWN,
    Space = VK_SPACE,
    F1 = VK_F1,
    C = 'C'
};

enum class KeyState
{
    None,
    Down,
    Press,
    Up
};

class InputManager : public Singleton<InputManager>
{
    friend class Singleton<InputManager>;

public:
    void Init(HWND window);
    void Update();

    bool GetButtonDown(KeyType key) const;
    bool GetButtonPressed(KeyType key) const;
    bool GetButtonUp(KeyType key) const;

private:
    InputManager() = default;
    ~InputManager() = default;

    KeyState GetState(KeyType key) const;

    static constexpr uint32 KEY_COUNT = 256;
    HWND _window = nullptr;
    std::array<KeyState, KEY_COUNT> _states{};
};
