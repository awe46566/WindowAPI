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
	UpdateJump(deltaTime);
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
				if (_jumpState == JumpState::AirBorne)
				{
					OnLanded();
				}
				break;
			}
		}
	}

	SetPosition(nextPosition);
}

void Player::UpdateJump(float deltaTime)
{
	// 충전 시작, 충전량 증가, 키를 놓았을 때 점프 실행	
	InputManager& input = InputManager::GetInstance();

	switch (_jumpState)
	{
		case JumpState::Ready:
			if (input.GetButtonDown(KeyType::Space))
			{				
				_jumpChargeTime = 0.0f;
				_jumpChargeStep = 1;
				_jumpState = JumpState::Charging;				
			}
			break;

		case JumpState::Charging:
			if (input.GetButtonPressed(KeyType::Space))
			{				
				_jumpChargeTime += deltaTime;
				//점프 충전 최대치 설정
				_jumpChargeTime = min(_jumpChargeTime, MAX_JUMP_CHARGE_TIME);
				//점프 충전 게이지 1~35단계
				_jumpChargeStep = 1 + static_cast<int>((_jumpChargeTime / MAX_JUMP_CHARGE_TIME) * (MAX_CHARGE_STEP - 1));
			}

  			if (input.GetButtonUp(KeyType::Space))
			{
				StartJump(deltaTime);
				_jumpState = JumpState::AirBorne;
			}
			break;

		case JumpState::AirBorne:
			//공중에 있을때 입력 막기
			break;
	}
}

void Player::StartJump(float deltaTime)
{
	//점프 충전량과 좌우 방향에 따라 velocity 설정
	const float chargeRatio = static_cast<float>(_jumpChargeStep - 1) / static_cast<float>(MAX_CHARGE_STEP - 1);
	const float jumpSpeed = MIN_JUMP_SPEED + (MAX_JUMP_SPEED - MIN_JUMP_SPEED) * chargeRatio;

	_verticalVelocity = -jumpSpeed;
}

void Player::OnLanded()
{
	_jumpChargeTime = 0.0f;
	_jumpChargeStep = 1;
	_verticalVelocity = 0.0f;
	_jumpState = JumpState::Ready;
}
