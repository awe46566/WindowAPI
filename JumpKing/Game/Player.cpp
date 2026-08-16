#include "pch.h"
#include "Player.h"
#include "Engine/SpriteRenderer.h"
#include "Engine/InputManager.h"
#include "Engine/GameConstants.h"
using namespace GameConstants;
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
	_collider->SetSize(Vector2{ PLAYER_COLLIDER_WIDTH, PLAYER_COLLIDER_HEIGHT });

	const float offsetX = (PLAYER_SPRITE_CELL_SIZE - PLAYER_COLLIDER_WIDTH) / 2.0f;
	const float offsetY = PLAYER_SPRITE_CELL_SIZE - PLAYER_COLLIDER_HEIGHT;
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

	if (_isStunned)
	{
		UpdateStun(deltaTime);
	}
	else
	{
		UpdateJump(deltaTime);
		Move(deltaTime);
	}
	
	ApplyWind(deltaTime);
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

	bool isLeftPressed = input.GetButtonPressed(KeyType::Left) || input.GetButtonDown(KeyType::Left);
	bool isRightPressed = input.GetButtonPressed(KeyType::Right) || input.GetButtonDown(KeyType::Right);

	if (_jumpState == JumpState::Ready)
	{	
		_isMoveInputPressed = isLeftPressed || isRightPressed;
		
		if (_groundSlope.x != 0.0f || _groundSlope.y != 0.0f)
		{
			ApplySlopeSlide(deltaTime);
		}
		else
		{
			// sprite FlipX
			if (isLeftPressed && !isRightPressed) _spriteRenderer->setFlipX(true);
			else if (isRightPressed && !isLeftPressed) _spriteRenderer->setFlipX(false);

			if (isLeftPressed && !isRightPressed)
			{
				_velocity.x = -PLAYER_MOVE_SPEED;
			}
			else if (isRightPressed && !isLeftPressed)
			{
				_velocity.x = PLAYER_MOVE_SPEED;
			}
			// 입력 없으면 아무것도 안 함 - 감속은 VerticalCollision의 매 프레임 slip이 담당
		}

	}
	else if (_jumpState == JumpState::Charging)
	{
		if (_groundSlope.x != 0.0f || _groundSlope.y != 0.0f)
		{
			ApplySlopeSlide(deltaTime);
		}
		// 평지에서는 아무것도 안 함 - King.py도 차징 중엔 _walk를 호출하지 않고
		// 잔여 속도를 매 프레임 slip에 맡긴다 (일반 바닥은 다음 프레임 즉시 0, 얼음은 서서히 감쇠)
	}

	nextPosition.x += _velocity.x * deltaTime;

	HorizontalCollision(nextPosition);
	SetPosition(nextPosition);
}

void Player::ApplyWind(float deltaTime)
{
	if (!_isGrounded || _groundMaterial != PlatformMaterial::Snow)
	{
		_velocity.x += _windForceX * deltaTime;
	}
}

void Player::ApplySlopeSlide(float deltaTime)
{
	// 입력 무시, 내리막 가속 + 재질별 마찰
	float downhillSign = (_groundSlope.x > 0.0f) ? -1.0f : 1.0f;
	_velocity.x += downhillSign * PLAYER_SLOPE_SLIDE_ACCEL * deltaTime;

	// 재질별 마찰 감속 (0을 지나쳐 역방향으로 뒤집히지 않도록 가드)
	float slopeFriction = (_groundMaterial == PlatformMaterial::Ice)
		? PLAYER_SLOPE_ICE_FRICTION
		: PLAYER_SLOPE_FRICTION;
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
		-PLAYER_SLOPE_MAX_SLIDE_SPEED,
		PLAYER_SLOPE_MAX_SLIDE_SPEED);
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
		position.x -= PLAYER_NOCLIP_SPEED * deltaTime;
	}
	else if (isRightPressed && !isLeftPressed)
	{
		position.x += PLAYER_NOCLIP_SPEED * deltaTime;
	}

	if (isUpPressed && !isDownPressed)
	{
		position.y -= PLAYER_NOCLIP_SPEED * deltaTime;
	}
	else if (isDownPressed && !isUpPressed)
	{
		position.y += PLAYER_NOCLIP_SPEED * deltaTime;
	}

	SetPosition(position);
}

void Player::ApplyGravity(float deltaTime)
{
	Vector2 nextPosition = GetPosition();

	// 매 프레임 리셋하고, VerticalCollision에서 바닥에 닿았을 때 다시 세팅한다.
	_isGrounded = false;

	// 화면 좌표에서는 y가 증가할수록 아래쪽이므로 중력은 양수 방향
	_velocity.y += PLAYER_GRAVITY * deltaTime;

	// 축별이 아니라 전체 크기 기준으로 상한 클램프 (파이썬의 전역 maxSpeed와 동일한 방식)
	float speed = hypotf(_velocity.x, _velocity.y);
	if (speed > PLAYER_MAX_FALL_SPEED)
	{
		float scale = PLAYER_MAX_FALL_SPEED / speed;
		_velocity.x *= scale;
		_velocity.y *= scale;
	}

	nextPosition.y += _velocity.y * deltaTime;
	VerticalCollision(nextPosition, deltaTime);

	SetPosition(nextPosition);

	if (!_isGrounded && _jumpState == JumpState::Ready)
	{
		_jumpState = JumpState::AirBorne;
	}
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
				_jumpAngle = 0.0f;
				_jumpState = JumpState::Charging;				
			}
			break;

		case JumpState::Charging:
			if (input.GetButtonPressed(KeyType::Space))
			{
				//방향 설정
				ChargingDirection();

				_jumpChargeTime += deltaTime;

				// King.py:335-341 오토파이어 - 최대 차지(jumpCount=30에 대응)를 넘기면
				// SPACE를 떼지 않아도 이번 프레임의 방향으로 즉시 발사
				if (_jumpChargeTime > PLAYER_MAX_JUMP_CHARGE_TIME)
				{
					_jumpChargeStep = PLAYER_MAX_JUMP_CHARGE_STEP;
					StartJump();
					_jumpState = JumpState::AirBorne;
					break;
				}

				//점프 충전 게이지 1~35단계
				_jumpChargeStep = 1 + static_cast<int>((_jumpChargeTime / PLAYER_MAX_JUMP_CHARGE_TIME) * (PLAYER_MAX_JUMP_CHARGE_STEP - 1));
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
	const float chargeRatio = static_cast<float>(_jumpChargeStep - 1) / static_cast<float>(PLAYER_MAX_JUMP_CHARGE_STEP - 1);

	//speed = 1.5 + (jumpCount/5)**1.13 을 그대로 이식 (jumpCount ≈ chargeRatio * 30)
	const float jumpCountEq = chargeRatio * PLAYER_JUMP_ANGLE_CHARGE_REFERENCE;
	float jumpSpeed = (1.5f + powf(jumpCountEq / 5.0f, PLAYER_JUMP_SPEED_CURVE_EXPONENT)) * 60.0f;

	//방향 점프 보너스 (+0.9 * 60fps)
	if (_jumpAngle != 0.0f)
		jumpSpeed += PLAYER_JUMP_DIRECTIONAL_SPEED_BONUS;

	jumpSpeed = min(jumpSpeed, PLAYER_MAX_JUMP_SPEED);

	// 기존 속도에 더함 (덮어쓰지 않음) - 착지 직전 잔여 속도가 다음 점프에 자연스럽게 섞임
	_velocity.x += sinf(_jumpAngle) * jumpSpeed;
	_velocity.y += -cosf(_jumpAngle) * jumpSpeed;
}

void Player::OnLanded()
{
	_collisionFlash = false;
	_jumpChargeTime = 0.0f;
	_jumpChargeStep = 1;

	_jumpAngle = 0.0f;
	_jumpState = JumpState::Ready;
}

void Player::ChargingDirection()
{
	InputManager& input = InputManager::GetInstance();

	bool isLeftPressed = input.GetButtonPressed(KeyType::Left) || input.GetButtonDown(KeyType::Left);
	bool isRightPressed = input.GetButtonPressed(KeyType::Right) || input.GetButtonDown(KeyType::Right);

	// 점프 일정 거리 이하면 수평 속도 조정
	float chargeRatio = (_jumpChargeStep - 1) / float(PLAYER_MAX_JUMP_CHARGE_STEP - 1);
	float angleMagnitude = PLAYER_JUMP_MAX_ANGLE_RADIANS
		* (1.0f - (chargeRatio * PLAYER_JUMP_ANGLE_CHARGE_REFERENCE) / PLAYER_JUMP_ANGLE_CHARGE_DIVISOR);

	if (isLeftPressed && !isRightPressed)
	{
		_jumpAngle = -angleMagnitude;
		_spriteRenderer->setFlipX(true);
	}
	else if (isRightPressed && !isLeftPressed)
	{
		_jumpAngle = angleMagnitude;
		_spriteRenderer->setFlipX(false);
	}
	else
	{
		_jumpAngle = 0.0f;
	}
}

void Player::HorizontalCollision(Vector2& nextPosition)
{
	// 슬로프에 서 있는 동안은 인접한 flat 채움 타일에 wall-bounce로 걸리지 않게 무시한다.
	// (수직 방향 제어는 ResolveSlopeCollision이 전담)
	if (_isGrounded && (_groundSlope.x != 0.0f || _groundSlope.y != 0.0f))
		return;

	//수평 충돌 체크
	if (_platforms != nullptr)
	{
		bool wallBounced = false;   // 이음매에서 여러 플랫폼이 동시에 걸려도 반사는 프레임당 한 번만

		for (const PlatformData& platform : *_platforms)
		{
			if (platform.hasSlope)
			{
				// 오르막 경사면은 걸어서 올라탈 수 없게 벽처럼 막는다 (점프로 넘어가거나
				// 위에서 착지하는 건 그대로 허용 - 공중에 있을 때는 건드리지 않는다).
				if (!_isGrounded || !IsSlopeFloor(platform))
					continue;

				bool risingRight = IsSlopeRisingRight(platform);
				bool movingIntoClimb =
					(risingRight && _velocity.x > 0.0f) ||
					(!risingRight && _velocity.x < 0.0f);

				if (!movingIntoClimb)
					continue;
			}

			const Rect nextBounds =
				_collider->GetBounds(nextPosition);

			HitResult hit;

			if (CollisionManager::GetInstance().CheckAABBToAABB(nextBounds, platform.bounds, hit))
			{
				// 수평면 충돌만 처리
				if (hit.normal.x != 0.0f)
				{
					nextPosition.x += hit.normal.x * hit.depth;

					// 공중에 있을 때만 반사한다.
					// 땅에서 걷다가 벽에 막히는 건 위치만 보정되고 속도 반사는 없음.
					if (!_isGrounded && !wallBounced)
					{
						wallBounced = true;

						// 각도를 접어서 반사 (단순 축 반전이 아님).
						// 그 순간의 velocity에서만 잠깐 각도/크기를 뽑아 쓰고 바로 velocity로 되돌린다 (지속 상태 없음).
						float angle = atan2f(_velocity.x, -_velocity.y);
						float speed = hypotf(_velocity.x, _velocity.y);

						if (-cosf(angle) <= 0.0f)
							angle = -angle * PLAYER_WALL_ANGLE_ELASTICITY;
						else
						{
							// (π - angle)을 그대로 빼면 angle이 -180° 경계를 넘을 때
							// 물리적으로는 같은 방향인데 결과가 반대쪽(위쪽)으로 튀는 문제가 있어,
							// 차이를 -180°~180°로 다시 감싼(wrap) 뒤 접는다.
							float diff = atan2f(sinf(angle - PI), cosf(angle - PI));
							angle = PI - diff * PLAYER_WALL_ANGLE_ELASTICITY;
						}

						speed *= PLAYER_WALL_BOUNCE_RESTITUTION;

						_velocity.x = sinf(angle) * speed;
						_velocity.y = -cosf(angle) * speed;

						if (!_isGrounded)
							_collisionFlash = true;
					}
				}
			}
		}
	}
}

void Player::VerticalCollision(Vector2& nextPosition, float deltaTime)
{
	if (_platforms == nullptr)
		return;

	// 슬로프를 먼저 처리해서, 램프 끝의 flat 채움 타일이 같은 프레임에
	// 겹치더라도 슬로프 착지 판정을 덮어쓰지 못하게 우선권을 준다.
	bool groundedOnSlope = false;

	for (const PlatformData& platform : *_platforms)
	{
		if (!platform.hasSlope)
			continue;

		if (ResolveSlopeCollision(platform, nextPosition, deltaTime))
			groundedOnSlope = true;
	}

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

		// 이번 프레임에 이미 슬로프에 착지했다면 flat 타일의 바닥 판정은 무시
		if (groundedOnSlope && hit.normal.y < 0.0f)
			continue;

		nextPosition.y += hit.normal.y * hit.depth;		

		// 위쪽으로 밀려났다면 플랫폼 위에 착지
		if (hit.normal.y < 0.0f)
		{
			if (hypotf(_velocity.x, _velocity.y) >= PLAYER_MAX_FALL_SPEED - 0.01f)
			{
				_isStunned = true;
				_stunTimer = PLAYER_STUN_DURATION;
			}

			_velocity.y = 0.0f;
			_isGrounded = true;
			_groundMaterial = platform.material;
			_groundSlope = { 0.0f, 0.0f };

			// 땅에 닿아있는 매 프레임 재질별 slip을 곱한다 (착지 순간만이 아님)
			float slip = (_groundMaterial == PlatformMaterial::Ice) ? PLAYER_LANDING_SLIP_ICE : PLAYER_LANDING_SLIP_NORMAL;
			_velocity.x *= slip;

			if (_jumpState == JumpState::AirBorne)
			{
				OnLanded();
			}
		}
		else
		{
			_velocity.y = -_velocity.y * PLAYER_CEILING_BOUNCE_RESTITUTION;
		}
	}
}

float Player::GetSlopeSurfaceY(const PlatformData& platform, float x) const
{
	// x를 타일 bounds 안으로 clamp하지 않는다: clamp하면 콜라이더 중심이
	// 타일 경계를 넘는 순간 표면이 그 지점 높이로 납작해져서, 경사가 갑자기
	// 끊긴 것처럼 보인다 (램프 끝에서 멈칫거리는 원인). ResolveSlopeCollision이
	// 이미 x 겹침 검사를 하고 나서만 이 함수를 부르므로, 여기서 벗어나는 범위는
	// 플레이어 콜라이더 폭 절반 정도로 제한된다 - 같은 타일의 대각선을 그만큼만
	// 자연스럽게 연장(extrapolate)한다.
	Vector2 left, right;

	GetSlopeEndpoints(platform, left, right);
	float t = (x - left.x) / (right.x - left.x);
	float y = left.y + t * (right.y - left.y);

	return y;
}

bool Player::ResolveSlopeCollision(const PlatformData& platform, Vector2& nextPosition, float deltaTime)
{
	const Rect nextBounds = _collider->GetBounds(nextPosition);

	if (nextBounds.Left() < platform.bounds.Right()
		&& nextBounds.Right() > platform.bounds.Left())
	{
		// 스냅 허용치를 고정 4px이 아니라 이번 프레임에 실제로 움직인 거리만큼
		// 넉넉하게 잡는다. 슬라이드 속도가 빠를 때 고정값으로는 한 프레임 만에
		// 표면에서 벗어나 접지가 끊겼다가(→AirBorne→OnLanded로 velocity.x가
		// 0으로 리셋) 다시 붙는 멈칫거림이 생겼었다.
		float frameTravel = (abs(_velocity.x) + abs(_velocity.y)) * deltaTime;
		float tolerance = max(PLAYER_SLOPE_SNAP_TOLERANCE, frameTravel);

		float centerX = (nextBounds.Right() + nextBounds.Left()) * 0.5f;

		// 이미 이 슬로프에 붙어서 이동 중이 아니라면, 콜라이더 중심이 실제로
		// 이 타일의 x 범위 안에 있을 때만 착지를 인정한다. 그렇지 않으면
		// 인접한 flat 타일 경계 근처에서 GetSlopeSurfaceY의 연장(extrapolation)된
		// 표면 때문에 flat 타일 위에 서 있는데도 슬로프에 착지한 것으로
		// 잘못 판정되어 미끄러지는 문제가 생긴다. 이미 붙어있던 슬로프라면
		// 기존처럼 경계 밖으로도 연장을 허용해 램프 반대쪽 끝에서 부드럽게 빠져나간다.
		bool alreadyOnThisSlope = _isGrounded
			&& _groundSlope.x == platform.slope.x
			&& _groundSlope.y == platform.slope.y;

		if (!alreadyOnThisSlope
			&& (centerX < platform.bounds.Left() || centerX > platform.bounds.Right()))
		{
			return false;
		}

		float surfaceY = GetSlopeSurfaceY(platform, centerX);
		float bottomDiff = surfaceY - nextBounds.Bottom();
		float topDiff = surfaceY - nextBounds.Top();

		if (IsSlopeFloor(platform))
		{
			if (bottomDiff >= -tolerance && bottomDiff <= tolerance)
			{
				// 속도를 표면 방향으로 맞춘다. 예전엔 여기서 velocity.y를 0으로 죽였는데,
				// 그러면 내려가는 움직임이 위치 스냅으로만 만들어져서, 슬로프를 벗어나는
				// 순간 vy=0인 채로 수평으로 튀어나가 궤적이 갑자기 꺾였다
				// (램프 끝에서 걸리듯 튕겨 보이던 원인).
				Vector2 left, right;
				GetSlopeEndpoints(platform, left, right);
				float gradient = (right.y - left.y) / (right.x - left.x);

				_velocity.y = _velocity.x * gradient;
				nextPosition.y += bottomDiff;

				_isGrounded = true;
				_groundMaterial = platform.material;
				_groundSlope = platform.slope;

				if (_jumpState == JumpState::AirBorne)
				{
					OnLanded();
				}

				return true;
			}
		}
		else if (topDiff >= -tolerance && topDiff <= tolerance)
		{
			_velocity.y = 0.0f;
			nextPosition.y += topDiff;
		}
	}

	return false;
}

void Player::UpdateStun(float deltaTime)
{
	_stunTimer -= deltaTime;
	if (_stunTimer <= 0.0f)
	{
		if (InputManager::GetInstance().GetButtonDown(KeyType::Left) ||
			InputManager::GetInstance().GetButtonDown(KeyType::Right) ||
			InputManager::GetInstance().GetButtonDown(KeyType::Space))
		{
			_isStunned = false;
		}		
	}
}

void Player::UpdateAnimation(float deltaTime)
{
	if (_spriteRenderer == nullptr)
	{
		return;
	}

	PlayerAnimState desired = PlayerAnimState::Idle;

	if (_isStunned)
	{
		desired = PlayerAnimState::Hurt;
	}
	else if (_collisionFlash)
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
		desired = (_isMoveInputPressed) ? PlayerAnimState::Move : PlayerAnimState::Idle;
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
			_spriteRenderer->ResetAnim(static_cast<int32>(PlayerAnimState::Move), true, PLAYER_MOVE_ANIM_DURATION, 3);
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

		case PlayerAnimState::Hurt:
			_spriteRenderer->SetFrame(static_cast<int32>(PlayerAnimState::Hurt), 0);
			break;

		default:
			break;
	}
}
