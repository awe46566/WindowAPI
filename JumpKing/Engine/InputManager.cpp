#include "pch.h"
#include "framework.h"
#include "Engine/InputManager.h"

void InputManager::Init(HWND window)
{
    _window = window;
    _states.fill(KeyState::None);
}

void InputManager::Update()
{
    BYTE keys[KEY_COUNT] = {};
    if (!GetKeyboardState(keys))
    {
        return;
    }

    for (uint32 key = 0; key < KEY_COUNT; ++key)
    {
        const bool isPressed = (keys[key] & 0x80) != 0;
        KeyState& state = _states[key];

        if (isPressed)
        {
            state = (state == KeyState::Down || state == KeyState::Press)
                ? KeyState::Press
                : KeyState::Down;
        }
        else
        {
            state = (state == KeyState::Down || state == KeyState::Press)
                ? KeyState::Up
                : KeyState::None;
        }
    }
}

bool InputManager::GetButtonDown(KeyType key) const
{
    return GetState(key) == KeyState::Down;
}

bool InputManager::GetButtonPressed(KeyType key) const
{
    return GetState(key) == KeyState::Press;
}

bool InputManager::GetButtonUp(KeyType key) const
{
    return GetState(key) == KeyState::Up;
}

KeyState InputManager::GetState(KeyType key) const
{
    return _states[static_cast<uint32>(key)];
}
