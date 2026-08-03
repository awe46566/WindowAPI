#include "pch.h"
#include "Player.h"
#include "SpriteRenderer.h"
#include "Engine/InputManager.h"
#include "ColliderAABB.h"
#include "Game/LevelData.h"

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
	Move(deltaTime);
	ApplyGravity(deltaTime);
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

void Player::ApplyGravity(float deltaTime)
{
	Vector2 previousPosition = GetPosition();
	Vector2 nextPosition = previousPosition;

	// 화면 좌표에서는 y가 증가할수록 아래쪽이므로 중력은 양수 방향
	_verticalVelocity += GRAVITY * deltaTime;
	_verticalVelocity = min(_verticalVelocity, MAX_FALL_SPEED);

	nextPosition.y += _verticalVelocity * deltaTime;

	if (_platforms != nullptr && _verticalVelocity >= 0.0f)
	{
		const Rect previousBounds = _collider->GetBounds(previousPosition);
		const Rect nextBounds = _collider->GetBounds(nextPosition);

		for (const PlatformData& platform : *_platforms)
		{
			if (platform.hasSlope) { continue; }			

			const Rect& platformBounds = platform.bounds;

			const bool overlapsHorizontally =
				nextBounds.Right() > platformBounds.Left() &&
				nextBounds.Left() < platformBounds.Right();

			const bool crossedPlatformTop =
				previousBounds.Bottom() <= platformBounds.Top() &&
				nextBounds.Bottom() >= platformBounds.Top();

			if (overlapsHorizontally && crossedPlatformTop)
			{
				nextPosition.y = platformBounds.Top() - nextBounds.height;
				_verticalVelocity = 0.0f;
				break;
			}
		}
	}

	SetPosition(nextPosition);
}
