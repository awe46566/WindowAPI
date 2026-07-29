#include "pch.h"
#include "UIButton.h"
#include "InputManager.h"

void UIButton::Update(float deltaTime)
{
	if (InputManager::GetInstance().GetButtonDown(KeyType::LeftMouse)
		&& ContainsPoint(InputManager::GetInstance().GetMousePos()))
	{
		if (_onClick)
		{
			_onClick();
		}
	}
}
