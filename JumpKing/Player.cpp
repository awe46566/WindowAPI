#include "pch.h"
#include "Player.h"
#include "SpriteRenderer.h"
#include "Engine/InputManager.h"
#include "ColliderAABB.h"

void Player::Init()
{
	wchar_t executablePath[MAX_PATH]{};
	const DWORD pathLength = GetModuleFileNameW(
		nullptr,
		executablePath,
		ARRAYSIZE(executablePath));

	if (pathLength == 0 || pathLength >= ARRAYSIZE(executablePath))
	{
		return;
	}

	const fs::path texturePath =
		fs::path(executablePath).parent_path()
		/ L".."
		/ L".."
		/ L"Resource"
		/ L"images"
		/ L"sheets"
		/ L"base.png";

	_spriteRenderer = AddComponent<SpriteRenderer>();
	if (_spriteRenderer->Load(texturePath.lexically_normal(), 9, 11))
	{
		// 어떤 행이 어떤 동작인지 정하기 전에는 첫 프레임만 표시합니다.
		_spriteRenderer->SetFrame(0, 7);
	}

	_collider = AddComponent<Collider>();
	_collider->SetSize(Vector2{ 32.0f, 40.0f });
}

void Player::Update(float deltaTime)
{
	Actor::Update(deltaTime);
	Player::Move(deltaTime);
}

void Player::Move(float deltaTime) 
{
	Vector2 position = GetPosition();
	InputManager& input = InputManager::GetInstance();

	bool isLeftPressed = input.GetButtonPressed(KeyType::Left) || input.GetButtonDown(KeyType::Left);
	bool isRightPressed = input.GetButtonPressed(KeyType::Right) || input.GetButtonDown(KeyType::Right);

	if (isLeftPressed)
	{
		position.x -= _moveSpeed * deltaTime;
	}

	if (isRightPressed)
	{
		position.x += _moveSpeed * deltaTime;
	}

	SetPosition(position);
}
