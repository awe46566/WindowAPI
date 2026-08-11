#include "pch.h"
#include "Player.h"
#include "Engine/SpriteRenderer.h"
#include "Engine/InputManager.h"
#include "Engine/GameConstants.h"
#include "Engine/DebugRenderer.h"
#include "Engine/ResourceCatalog.h"
#include "Framework/CollisionManager.h"
#include "Framework/ColliderAABB.h"
#include "Game/LevelData.h"

namespace
{
	// player_king.png의 한 칸 크기. 프레임 재생 시작 위치(row)와
	// 콜라이더 오프셋 계산에 공통으로 쓰인다.
	constexpr float PLAYER_SPRITE_CELL_SIZE = 36.0f;
}

void Player::Init()
{
	_spriteRenderer = AddComponent<SpriteRenderer>();

	const ImageResource* image = ResourceCatalog::GetInstance().FindImage("player_king");
	if (image != nullptr && _spriteRenderer->Load(image->path, image->rows, image->columns))
	{
		_spriteRenderer->SetFrame(static_cast<int32>(PlayerAnimState::Idle), 0);
	}

	_collider = AddComponent<ColliderAABB>();
	_collider->SetSize(Vector2{ GameConstants::PLAYER_COLLIDER_WIDTH, GameConstants::PLAYER_COLLIDER_HEIGHT });

	const float offsetX = (PLAYER_SPRITE_CELL_SIZE - GameConstants::PLAYER_COLLIDER_WIDTH) / 2.0f;
	const float offsetY = PLAYER_SPRITE_CELL_SIZE - GameConstants::PLAYER_COLLIDER_HEIGHT;
	_collider->SetOffset(Vector2{ offsetX, offsetY });
}

void Player::Update(float deltaTime)
{
	Actor::Update(deltaTime);

	if (InputManager::GetInstance().GetButtonDown(KeyType::C))
	{
		_noclip = !_noclip;
		if (!_noclip)
		{
			// 노클립 종료 시 정상 물리로 깨끗하게 복귀
			_velocity = { 0.0f, 0.0f };
			_jumpState = JumpState::Ready;
		}
	}

	if (_noclip)
	{
		UpdateNoclip(deltaTime);
		UpdateAnimation(deltaTime);
		return;
	}

	UpdateJump(deltaTime);
	Move(deltaTime);
	ApplyGravity(deltaTime);
	UpdateAnimation(deltaTime);
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
		bool isLeftPressed = input.GetButtonPressed(KeyType::Left) || input.GetButtonDown(KeyType::Left);
		bool isRightPressed = input.GetButtonPressed(KeyType::Right) || input.GetButtonDown(KeyType::Right);
		
		if (_groundSlope.x != 0.0f || _groundSlope.y != 0.0f)
		{
			// 입력 무시, 내리막 가속 + 재질별 마찰
			float downhillSign = (_groundSlope.x > 0.0f) ? -1.0f : 1.0f;
			_velocity.x += downhillSign * GameConstants::PLAYER_SLOPE_SLIDE_ACCEL * deltaTime;

			// 재질별 마찰 감속 (0을 지나쳐 역방향으로 뒤집히지 않도록 가드)
			float slopeFriction = (_groundMaterial == PlatformMaterial::Ice)
				? GameConstants::PLAYER_SLOPE_ICE_FRICTION
				: GameConstants::PLAYER_SLOPE_FRICTION;
			float frictionDelta = slopeFriction * deltaTime;

			if (_velocity.x > 0.0f)
			{
				_velocity.x = max(0.0f, _velocity.x - frictionDelta);
			}
			else if (_velocity.x < 0.0f)
			{
				_velocity.x = min(0.0f, _velocity.x + frictionDelta);
			}

			_velocity.x = clamp(_velocity.x,
				-GameConstants::PLAYER_SLOPE_MAX_SLIDE_SPEED,
				GameConstants::PLAYER_SLOPE_MAX_SLIDE_SPEED);
		}
		else if (_groundMaterial == PlatformMaterial::Ice)
		{
			// sprite FlipX
			if (isLeftPressed && !isRightPressed) _spriteRenderer->setFlipX(true);
			else if (isRightPressed && !isLeftPressed) _spriteRenderer->setFlipX(false);

			// 입력 방향으로 가속 + PLAYER_ICE_FRICTION으로 감쇠
		}
		else
		{
			// sprite FlipX
			if (isLeftPressed && !isRightPressed) _spriteRenderer->setFlipX(true);
			else if (isRightPressed && !isLeftPressed) _spriteRenderer->setFlipX(false);

			_velocity.x = 0.0f;
			if (isLeftPressed && !isRightPressed)
			{
				_velocity.x = -GameConstants::PLAYER_MOVE_SPEED;				
			}
			else if (isRightPressed && !isLeftPressed)
			{
				_velocity.x = GameConstants::PLAYER_MOVE_SPEED;				
			}
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

void Player::UpdateNoclip(float deltaTime)
{
	InputManager& input = InputManager::GetInstance();
	Vector2 position = GetPosition();

	bool isLeftPressed = input.GetButtonPressed(KeyType::Left) || input.GetButtonDown(KeyType::Left);
	bool isRightPressed = input.GetButtonPressed(KeyType::Right) || input.GetButtonDown(KeyType::Right);
	bool isUpPressed = input.GetButtonPressed(KeyType::Up) || input.GetButtonDown(KeyType::Up);
	bool isDownPressed = input.GetButtonPressed(KeyType::Down) || input.GetButtonDown(KeyType::Down);

	if (isLeftPressed && !isRightPressed)
	{
		position.x -= GameConstants::PLAYER_NOCLIP_SPEED * deltaTime;
	}
	else if (isRightPressed && !isLeftPressed)
	{
		position.x += GameConstants::PLAYER_NOCLIP_SPEED * deltaTime;
	}

	if (isUpPressed && !isDownPressed)
	{
		position.y -= GameConstants::PLAYER_NOCLIP_SPEED * deltaTime;
	}
	else if (isDownPressed && !isUpPressed)
	{
		position.y += GameConstants::PLAYER_NOCLIP_SPEED * deltaTime;
	}

	SetPosition(position);
}

void Player::ApplyGravity(float deltaTime)
{
	Vector2 previousPosition = GetPosition();
	Vector2 nextPosition = previousPosition;

	// 매 프레임 리셋하고, VerticalCollision에서 바닥에 닿았을 때 다시 세팅한다.
	_isGrounded = false;

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
	_collisionFlash = false;
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

			if (CollisionManager::GetInstance().CheckAABBToAABB(nextBounds, platform.bounds, hit))
			{
				// 수평면 충돌만 처리
				if (hit.normal.x != 0.0f)
				{
					nextPosition.x += hit.normal.x * hit.depth;

					// velocity의 부호를 뒤집어 방향 Bounce 힘만큼 반사
					_velocity.x = -_velocity.x * GameConstants::PLAYER_WALL_BOUNCE_RESTITUTION;

					_collisionFlash = true;
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
			{
				ResolveSlopeCollision(platform, nextPosition);
				continue;
			}

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
				_isGrounded = true;
				_groundMaterial = platform.material;
				_groundSlope = { 0.0f, 0.0f };

				if (_jumpState == JumpState::AirBorne)
				{
					OnLanded();
				}
			}
		}
	}
}

float Player::GetSlopeSurfaceY(const PlatformData& platform, float x) const
{
	// TODO : x를 [bounds.Left(), bounds.Right()]로 clamp한 뒤,
	// LevelData.h의 IsSlopeRisingRight/IsSlopeFloor 규칙대로
	// 대각선 위의 y를 선형보간해서 반환한다.
	x = max(platform.bounds.Left(), min(x, platform.bounds.Right()));

	Vector2 left, right;

	GetSlopeEndpoints(platform, left, right);
	float t = (x - left.x) / (right.x - left.x);
	float y = left.y + t * (right.y - left.y);

	return y;
}

bool Player::ResolveSlopeCollision(const PlatformData& platform, Vector2& nextPosition)
{
	// TODO 
	// 1) _collider->GetBounds(nextPosition)와 platform.bounds가 수평으로 겹치는지 확인
	// 2) 콜라이더 중심 x에서 GetSlopeSurfaceY로 표면 y 계산
	// 3) floor면 콜라이더 bottom을, ceiling이면 top을 표면에 스냅
	// 4) _velocity.y = 0, AirBorne 상태였다면 OnLanded() 호출
	// 5) floor에 착지했다면 _isGrounded=true, _groundMaterial=platform.material,
	//    _groundSlope=platform.slope 로 기록 (미끄러짐 로직이 여기서 참조함)
	const Rect nextBounds = _collider->GetBounds(nextPosition);
	
	if (nextBounds.Left() < platform.bounds.Right() 
		&& nextBounds.Right() > platform.bounds.Left())
	{
		float centerX = (nextBounds.Right() + nextBounds.Left()) * 0.5f;
		float surfaceY = GetSlopeSurfaceY(platform, centerX);
		float bottomDiff = surfaceY - nextBounds.Bottom();
		float topDiff = surfaceY - nextBounds.Top();

		if (IsSlopeFloor(platform))
		{
			if ( _velocity.y >= 0.0f &&
				bottomDiff >= -GameConstants::PLAYER_SLOPE_SNAP_TOLERANCE &&
				bottomDiff <= GameConstants::PLAYER_SLOPE_SNAP_TOLERANCE)
			{
				_velocity.y = 0.0f;
				nextPosition.y += bottomDiff;

				_isGrounded = true;
				_groundMaterial = platform.material;
				_groundSlope = platform.slope;

				if (_jumpState == JumpState::AirBorne)
				{
					OnLanded();
				}
			}
		}
		else if(_velocity.y <= 0.0f &&
			topDiff >= -GameConstants::PLAYER_SLOPE_SNAP_TOLERANCE &&
			topDiff <= GameConstants::PLAYER_SLOPE_SNAP_TOLERANCE)
		{						
			_velocity.y = 0.0f;
			nextPosition.y += topDiff;
		}
	}

	return false;
}

void Player::UpdateAnimation(float deltaTime)
{
	if (_spriteRenderer == nullptr)
	{
		return;
	}

	PlayerAnimState desired = PlayerAnimState::Idle;

	if (_collisionFlash)
	{
		desired = PlayerAnimState::Collision;
	}
	else if (_isGrounded && (_groundSlope.x != 0.0f || _groundSlope.y != 0.0f))
	{
		desired = PlayerAnimState::Collision;
	}
	else if (_jumpState == JumpState::Charging)
	{
		desired = PlayerAnimState::Charge;
	}
	else if (_jumpState == JumpState::AirBorne)
	{
		// 화면 좌표는 y가 증가할수록 아래쪽이므로, y속도가 음수면 상승 중
		desired = (_velocity.y < 0.0f) ? PlayerAnimState::Up : PlayerAnimState::Down;
	}
	else
	{
		desired = (_velocity.x != 0.0f) ? PlayerAnimState::Move : PlayerAnimState::Idle;
	}

	if (desired == _animState)
	{
		return;
	}
	_animState = desired;

	switch (_animState)
	{
		case PlayerAnimState::Idle:
			_spriteRenderer->SetFrame(static_cast<int32>(PlayerAnimState::Idle), 0);
			break;

		case PlayerAnimState::Move:
			_spriteRenderer->ResetAnim(static_cast<int32>(PlayerAnimState::Move), true, GameConstants::PLAYER_MOVE_ANIM_DURATION, 3);
			break;

		case PlayerAnimState::Charge:
			_spriteRenderer->SetFrame(static_cast<int32>(PlayerAnimState::Charge), 0);
			break;

		case PlayerAnimState::Collision:
			_spriteRenderer->SetFrame(static_cast<int32>(PlayerAnimState::Collision), 0);
			break;

		case PlayerAnimState::Up:
			_spriteRenderer->SetFrame(static_cast<int32>(PlayerAnimState::Up), 0);
			break;

		case PlayerAnimState::Down:
			_spriteRenderer->SetFrame(static_cast<int32>(PlayerAnimState::Down), 0);
			break;

		default:
			break;
	}
}
