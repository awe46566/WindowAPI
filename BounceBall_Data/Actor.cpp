#include "pch.h"
#include "Actor.h"
#include "Game.h"
#include "Scene.h"
#include "Component.h"
#include "SceneManager.h"
#include "Collider.h"

Actor::~Actor()
{
	// new Component 들.. 메모리 해제.
	for (auto component : _components)
	{
		delete component;
	}
	_components.clear();
}

void Actor::Init()
{

}

// Actor 파괴(삭제) 싶으면, 무조건 Scene에 예약을 걸어서 처리한다.
void Actor::Destroy()
{
	// 1번 방식
	//_pendingKill = true; // 삭제 예약 상태 flag 추가해도 된다.
	
	// 2번 방식
	SceneManager::GetInstance().GetScene()->DeleteActor(this);
}

void Actor::Update(float deltaTime)
{
	for (auto component : _components)
	{
		component->Update(deltaTime);
	}
}

void Actor::Render(HDC hdc)
{
	for (auto component : _components)
	{
		component->Render(hdc, GetPos());
	}
}

// 위치가 변경되었으니 grid 갱신도 같이 해주자.
void Actor::SetPos(Vector pos, bool applyGrid)
{
	// 이전 좌표를 잠깐 저장해두고
	Vector prevPos = _pos;


	// 좌표가 바뀔때는 무조건 SetPos() 함수를 통해서 들어온다.
	_pos = pos;

	if (applyGrid)
	{
		SceneManager::GetInstance().GetScene()->UpdateGrid(this, prevPos);
	}
}

void Actor::cacheCollider(Component* component)
{
	Collider* collider = dynamic_cast<Collider*>(component);
	if (collider)
	{
		_collider = collider;	// 한번 캐싱해둔다.
	}
}
