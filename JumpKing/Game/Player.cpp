#include "pch.h"
#include "Player.h"
#include "Engine/SpriteRenderer.h"
#include "Engine/InputManager.h"
#include "Engine/GameConstants.h"
#include "Engine/DebugRenderer.h"
#include "Framework/CollisionManager.h"
#include "Framework/ColliderAABB.h"
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


	_collider = AddComponent<ColliderAABB>();
	_collider->SetSize(Vector2{ GameConstants::PLAYER_COLLIDER_WIDTH, GameConstants::PLAYER_COLLIDER_HEIGHT });

	const float offsetX = (32.0f - GameConstants::PLAYER_COLLIDER_WIDTH) / 2.0f;
	const float offsetY = 40.0f - GameConstants::PLAYER_COLLIDER_HEIGHT;
	_collider->SetOffset(Vector2{ offsetX, offsetY });
}

void Player::Update(float deltaTime)
{
	UpdateJump(deltaTime);
	Move(deltaTime);
	ApplyGravity(deltaTime);
}

void Player::Render(const RenderContext& context)
{
	Actor::Render(context);

	if (_collider != nullptr)
	{
		const Rect bounds = _collider->GetBounds(GetPosition());

		DebugRenderer::DrawRect(context, bounds, D2D1::ColorF(D2D1::ColorF::Red));
	
	}
}


void Player::Move(float deltaTime) 
{
	Vector2 position = GetPosition();
	Vector2 nextPosition = position;
	InputManager& input = InputManager::GetInstance();

	if (_jumpState == JumpState::Ready)
	{
		_velocity.x = 0.0f;

		bool isLeftPressed = input.GetButtonPressed(KeyType::Left) || input.GetButtonDown(KeyType::Left);
		bool isRightPressed = input.GetButtonPressed(KeyType::Right) || input.GetButtonDown(KeyType::Right);
	

		if (isLeftPressed && !isRightPressed)
		{
			_velocity.x = -GameConstants::PLAYER_MOVE_SPEED;
			_spriteRenderer->setFlipX(true);
		}
		else if (isRightPressed && !isLeftPressed)
		{
			_velocity.x = GameConstants::PLAYER_MOVE_SPEED;
			_spriteRenderer->setFlipX(false);
		}

	}
	else if (_jumpState == JumpState::Charging)
	{
		// 차징 상태는 정지
		_velocity.x = 0.0f;
	}

	nextPosition.x += _velocity.x * deltaTime;

	HorizontalCollision(nextPosition);
	SetPosition(nextPosition);
}

void Player::ApplyGravity(float deltaTime)
{
	Vector2 previousPosition = GetPosition();
	Vector2 nextPosition = previousPosition;

	// 화면 좌표에서는 y가 증가할수록 아래쪽이므로 중력은 양수 방향
	_velocity.y += GameConstants::PLAYER_GRAVITY * deltaTime;
	_velocity.y = min(_velocity.y, GameConstants::PLAYER_MAX_FALL_SPEED);

	nextPosition.y += _velocity.y * deltaTime;

	VerticalCollision(nextPosition);
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
				_jumpDirection = { 0.0f, -1.0f };
				_jumpState = JumpState::Charging;				
			}
			break;

		case JumpState::Charging:
			if (input.GetButtonPressed(KeyType::Space))
			{				
				//방향 설정
				ChargingDirection();

				//점프 충전 최대치 설정
				_jumpChargeTime += deltaTime;		
				_jumpChargeTime = min(_jumpChargeTime, GameConstants::PLAYER_MAX_JUMP_CHARGE_TIME);
				//점프 충전 게이지 1~35단계
				_jumpChargeStep = 1 + static_cast<int>((_jumpChargeTime / GameConstants::PLAYER_MAX_JUMP_CHARGE_TIME) * (GameConstants::PLAYER_MAX_JUMP_CHARGE_STEP - 1));
				
			}

  			if (input.GetButtonUp(KeyType::Space))
			{
				StartJump();
				_jumpState = JumpState::AirBorne;
			}
			break;

		case JumpState::AirBorne:			
			break;
	}
}

void Player::StartJump()
{
	//점프 충전량과 좌우 방향에 따라 velocity 설정
	const float chargeRatio = static_cast<float>(_jumpChargeStep - 1) / static_cast<float>(GameConstants::PLAYER_MAX_JUMP_CHARGE_STEP - 1);
	const float jumpSpeed = GameConstants::PLAYER_MIN_JUMP_SPEED + (GameConstants::PLAYER_MAX_JUMP_SPEED - GameConstants::PLAYER_MIN_JUMP_SPEED) * chargeRatio;

	_velocity.x = _jumpDirection.x * jumpSpeed;
	_velocity.y = _jumpDirection.y * jumpSpeed;
}

void Player::OnLanded()
{
	_jumpChargeTime = 0.0f;
	_jumpChargeStep = 1;
	_velocity = { 0.0f, 0.0f };
	_jumpDirection = { 0.0f, -1.0f };
	_jumpState = JumpState::Ready;
}

void Player::ChargingDirection()
{
	InputManager& input = InputManager::GetInstance();

	bool isLeftPressed = input.GetButtonPressed(KeyType::Left) || input.GetButtonDown(KeyType::Left);
	bool isRightPressed = input.GetButtonPressed(KeyType::Right) || input.GetButtonDown(KeyType::Right);

	// 점프 일정 거리 이하면 수평 속도 조정
	float horizontalDirection = (_jumpChargeStep < 21) ? GameConstants::PLAYER_LOW_HORIZONTAL_JUMP_DIRECTION : GameConstants::PLAYER_HORIZONTAL_JUMP_DIRECTION;

	if (isLeftPressed && !isRightPressed)
	{
		_jumpDirection = { -horizontalDirection, GameConstants::PLAYER_VERTICAL_JUMP_DIRECTION };
		_spriteRenderer->setFlipX(true);
	}
	else if (isRightPressed && !isLeftPressed)
	{
		_jumpDirection = { horizontalDirection, GameConstants::PLAYER_VERTICAL_JUMP_DIRECTION };
		_spriteRenderer->setFlipX(false);
	}
	else
	{
		_jumpDirection = { 0.0f, -1.0f };
	}

	_jumpDirection.Normalize();
}

void Player::HorizontalCollision(Vector2& nextPosition)
{
	//수평 충돌 체크
	if (_platforms != nullptr)
	{
		for (const PlatformData& platform : *_platforms)
		{
			if (platform.hasSlope)
				continue;

			const Rect nextBounds =
				_collider->GetBounds(nextPosition);

			HitResult hit;

			if (CollisionManager::GetInstance().CheckAABBToAABB(
				nextBounds,
				platform.bounds,
				hit))
			{
				// 수평면 충돌만 처리
				if (hit.normal.x != 0.0f)
				{
					nextPosition.x += hit.normal.x * hit.depth;
					_velocity.x = 0.0f;
				}
			}
		}
	}
}

void Player::VerticalCollision(Vector2& nextPosition)
{
	//수직 충돌 체크
	if (_platforms != nullptr)
	{
		for (const PlatformData& platform : *_platforms)
		{
			if (platform.hasSlope)
				continue;

			const Rect nextBounds =
				_collider->GetBounds(nextPosition);

			HitResult hit;

			if (!CollisionManager::GetInstance().CheckAABBToAABB(
				nextBounds,
				platform.bounds,
				hit))
			{
				continue;
			}

			// 바닥 또는 천장 충돌만 처리
			if (hit.normal.y == 0.0f)
				continue;

			nextPosition.y += hit.normal.y * hit.depth;
			_velocity.y = 0.0f;

			// 위쪽으로 밀려났다면 플랫폼 위에 착지
			if (hit.normal.y < 0.0f)
			{
				if (_jumpState == JumpState::AirBorne)
				{
					OnLanded();
				}
			}
		}
	}
}
