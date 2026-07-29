#include "pch.h"
#include "Ball.h"
#include "ImageRenderer.h"
#include "ColliderCircle.h"
#include "InputManager.h"
#include "SoundManager.h"
#include "SceneManager.h"
#include "GameScene.h"
#include "IPickupable.h"

void Ball::Init()
{
	ImageRenderer* renderer = AddComponent<ImageRenderer>();
	renderer->Init(L"Ball");
	renderer->SetSize(BALL_SIZE, BALL_SIZE);

	// 원형 충돌체 추가
	ColliderCircle* collider = AddComponent<ColliderCircle>();
	collider->Init(this, BALL_SIZE / 2);
	collider->SetCheckCell(true);
}

void Ball::Update(float deltaTime)
{
	Super::Update(deltaTime);

	if (_editorScene == true)
		return;

	// 가속도는 속도의 변화량
	// 매프레임 속도가 어떻게 변화하는지를 계산하면 된다.
	Vector acceleration;

	// 가속도 변화량 코드가 들어갈 예정.
	if (InputManager::GetInstance().GetButtonPressed(KeyType::Left))
	{
		// 가속도 = 힘 / 질량
		acceleration.x -= _sideForce / _mass;
	}
	if (InputManager::GetInstance().GetButtonPressed(KeyType::Right))
	{
		acceleration.x += _sideForce / _mass;
	}

	// 중력 가속도 적용
	acceleration.y += _gravity;

	// 공이 지속적으로 아래로 떨어지면 된다.
	_velocity += acceleration * deltaTime;

	// 수평 이동에 대한 마찰력을 적용한다.
	// 마찰력도 원하는 그래프 감속 느낌에 따라 공식을 적용하면 된다.
	// 선형 감소, 지수 감소...
	_velocity.x -= _velocity.x * _sideFriction * deltaTime;

	// veclocity : 위치변화량(속도)이 계산되면, 위치값을 갱신해준다.
	Vector pos = GetPos();
	pos += _velocity * deltaTime;
	SetPos(pos);

	if (pos.y > GWinSizeY)
	{
		// 범위를 벗어났으니 죽음 처리
		GameScene* gameScene = dynamic_cast<GameScene*>(SceneManager::GetInstance().GetScene());
		gameScene->Dead(GetPos());
	}
}

void Ball::OnEnter(Actor* other, const HitResult& hit)
{
	// IPickupable
	// IInteractive
	if (IPickupable* pickupable = dynamic_cast<IPickupable*>(other))
	{
		pickupable->OnPickedUp(this);
		return;
	}

	// 추후에 아이템이 들어가면 if/else 예외처리해야한다...
	//if (other->GetActorType() == ActorType::Item)
	//{
	//	// 아이템을 획득하는 처리
	//	return;
	//}


	if (other->GetActorType() != ActorType::Block)
		return;

	// 사운드 재생해주세요.
	SoundManager::GetInstance().Play(L"");

	// 겹친 길이만큼 보정해서, 공이 블럭을 파고들지 않게 후보정한다.
	SetPos(GetPos() + hit.normal * (hit.depth));

	// 블럭과 부딪히는 순간, 공이 튀어오른다.
	_velocity = _velocity.Reflect(hit.normal);

	// 바운스볼 특성상 당구공같은 물리가 아니라, 늘 일정한 속도로 튀어오르는 공을 만들어야 한다.
	if (hit.normal.y < 0.0f)
	{
		_velocity.y = _bounceSpeedY;
	}

	// 양쪽벽에 부딪혔을때 부딪힌 방향의 normal 값만큼 방향 벡터 계산해서 벗어나게 한다.
	if (abs(hit.normal.x) > 0)
	{
		_velocity.x = hit.normal.x * _bounceSpeedX;
	}
}

void Ball::OnStay(Actor* other, const HitResult& hit)
{
	if (other->GetActorType() != ActorType::Block)
		return;

	// 겹친 길이만큼 보정해서, 공이 블럭을 파고들지 않게 후보정한다.
	SetPos(GetPos() + hit.normal * (hit.depth));

	// 다음프레임에서 velocity 에 의해 자꾸 파고들지 않게 하려고
	// 파고드는 힘 자체도 제거. velocity에서 수직운동만 남긴다.
	// 파고드는 힘(velocity)값이 벽이랑 부딪혔을때, 실제로 음수의 효율크기만 남아서
	// 음수인지 확인하고, 해당 효율만큼 제거한다.
	float vn = _velocity.Dot(hit.normal);
	if (vn < 0.f)
		_velocity -= hit.normal * vn;
}

void Ball::OnExit(Actor* other)
{
}
