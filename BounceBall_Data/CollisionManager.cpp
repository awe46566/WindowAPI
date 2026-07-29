#include "pch.h"
#include "CollisionManager.h"
#include "Game.h"
#include "Scene.h"
#include "Actor.h"
#include "ColliderCircle.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "ColliderAABB.h"


namespace DispatchTable
{
	bool CircleToCircle(Collider* a, Collider* b, HitResult& result)
	{
		return Collider::CheckCircleToCircle(static_cast<ColliderCircle*>(a), static_cast<ColliderCircle*>(b), result);
	}

	bool CircleToAABB(Collider* a, Collider* b, HitResult& result)
	{
		return Collider::CheckCircleToAABB(static_cast<ColliderCircle*>(a), static_cast<ColliderAABB*>(b), result);
	}

	bool AABBToCircle(Collider* a, Collider* b, HitResult& result)
	{
		return Collider::CheckCircleToAABB(static_cast<ColliderCircle*>(b), static_cast<ColliderAABB*>(a), result);
	}

	bool AABBToAABB(Collider*, Collider*, HitResult& result)
	{
		return false; // AABB vs AABB 는 아직 미지원 (필요해지면 여기만 채우면 된다)
	}
}

void CollisionManager::Init()
{
	// collider Type에 맞춰서 호출해야하는 함수를 결정
	DISPATCH_TABLE[(int32)ColliderType::Circle][(int32)ColliderType::Circle] = DispatchTable::CircleToCircle;
	DISPATCH_TABLE[(int32)ColliderType::Circle][(int32)ColliderType::AABB] = DispatchTable::CircleToAABB;

	DISPATCH_TABLE[(int32)ColliderType::AABB][(int32)ColliderType::Circle] = DispatchTable::AABBToCircle;
	DISPATCH_TABLE[(int32)ColliderType::AABB][(int32)ColliderType::AABB] = DispatchTable::AABBToAABB;



	// 여긴 충돌체크 필요없는 케이스를 테이블화
	// "돌+나무", "나무+돌"

	/* 위쪽 코드처럼 함수로 분리시, 간결해진다.
	IGNORE_MASK[(int32)ActorType::Enemy][(int32)ActorType::Enemy] = true;
	IGNORE_MASK[(int32)ActorType::Player][(int32)ActorType::Player] = true;

	IGNORE_MASK[(int32)ActorType::Enemy][(int32)ActorType::EnemyBullet] = true;
	IGNORE_MASK[(int32)ActorType::EnemyBullet][(int32)ActorType::Enemy] = true;

	IGNORE_MASK[(int32)ActorType::Player][(int32)ActorType::PlayerBullet] = true;
	IGNORE_MASK[(int32)ActorType::PlayerBullet][(int32)ActorType::Player] = true;

	IGNORE_MASK[(int32)ActorType::EnemyBullet][(int32)ActorType::PlayerBullet] = true;
	IGNORE_MASK[(int32)ActorType::PlayerBullet][(int32)ActorType::EnemyBullet] = true;
	*/
}

void CollisionManager::Clear()
{
	_prev.clear();
	_curr.clear();
	_collisionCheckList.clear();
}

void CollisionManager::Update()
{
	// 현재 상태에 대한 충돌체크만 수행해서 결과를 저장
	_curr.clear();

	// 충돌체크가 필요한 Actor는 전부다 비교해서 충돌체크를 수행한다.
	// 일단, 모든 녀석들을 다 순회하면서 체크한다.
	// [0] : player, [1] enemy, [2] p.bullet [3] e.bullet ..
	// [0]<->[1], [2], [3] 전부 비교

	// _collisionCheckList : 리스트에 존재한다는건, 충돌체크를 실행해야할 '주체'
	// 현재 : 내 비행기, 적 비행기, 내 총알, 적 총알
	// Grid 방식 : 내 비행기, 내 총알, 

	// 주체가 되는 녀석들만 순회
	for (auto actor : _collisionCheckList)
	{
		// 어떤 대상과 충돌체크를 해야하냐면, 내가 있는 셀과 인접한 셀만 충돌체크 수행
		checkCollision(actor);
	}

	// 현재 프레임에 충돌체크가 필요한 상태 체크 완료
	// Exit 
	for (const auto& iter : _prev)
	{
		// 이전에는 있었는데, 현재는 없다.
		// Exit
		if (_curr.contains(iter) == false)
		{
			// pair<Actor*, Actor*>
			// 양방향으로 Exit 함수를 호출해준다.
			iter.first->OnExit(iter.second);
			iter.second->OnExit(iter.first);
		}
	}

	// curr -> prev
	_prev = _curr;	// 이제부터 curr 상태가 prev 상태로 변경.
	//swap(_prev, _curr);

	// 디버깅 정보 토글
	if (InputManager::GetInstance().GetButtonDown(KeyType::F1))
	{
		//if (_drawDebug)
		//{
		//	_drawDebug = false;
		//}
		//else
		//{
		//	_drawDebug = true;
		//}

		_drawDebug = !_drawDebug;
	}
}

void CollisionManager::Render(HDC hdc)
{
	//if (_drawDebug)
	//{
	//	// 그리드 라인 보기
	//	drawGridLine(hdc);

	//	// 디버깅을 위한 충돌체 상태 보기
	//	for (auto actor : _collisionCheckList)
	//	{
	//		actor->GetCollider()->Render(hdc, actor->GetPos());
	//	}
	//}
}

void CollisionManager::AddActor(Actor* actor)
{
	// 적비행기, 적총알은 등록(X)
	if (actor->GetCollider() && actor->GetCollider()->CheckCell())
	{
		// 충돌체크가 필요한 객체 추가
		_collisionCheckList.push_back(actor);
	}
}

void CollisionManager::RemoveActor(Actor* actor)
{
	// 충돌체크가 필요한 객체에서 제거
	std::erase_if(_collisionCheckList, [actor](const Actor* iter) 
		{
			return iter == actor;
		});


	// 제거해야할 대상을 걸러주는 람다식
	auto checkActor = [actor](const std::pair<Actor*, Actor*>& pair)
		{
			if (pair.first == actor || pair.second == actor)
				return true;
			return false;
		};

	// 전체순회해도 비용이 크지 않다.
	// 겹쳐져있는 대상만 set 에 추가될꺼에요.
	std::erase_if(_prev, checkActor);
	std::erase_if(_curr, checkActor);
}

void CollisionManager::addOverlapState(Actor* actor1, Actor* actor2, const HitResult& result)
{
	//  항상 원하는 순서대로 정렬해서 key 를 만들자.
	// "돌"+"나무" => "돌"+"나무"
	// "나무"+"돌" => "돌"+"나무"
	// * point : 일종의 주소값 0x001 < 0x002 (8byte)
	// actor1 : 0x001
	// actor2 : 0x002
	// key -> pair(0x001, 0x002)

	// actor1 : 0x002
	// actor2 : 0x001
	// key -> pair(actor2, actor1)

	auto pair = (actor1 < actor2) ? make_pair(actor1, actor2) : make_pair(actor2, actor1);

	// 현재 프레임에 충돌상태 체크 됨
	bool insert = _curr.insert(pair).second;	// second : true, 중복된 키를 추가했으면, second : false
	bool prev = _prev.contains(pair); // 이전 프레임에 key 조합이 있었는지 확인

	if (insert == true && prev == false)
	{
		actor1->OnEnter(actor2, result);
		actor2->OnEnter(actor1, result);
	}
	// Stay 해보고 싶으면
	// 현재 insert == true, 이전 : true
	else if (insert == true && prev == true)
	{
		// Stay : 이전 프레임에도 겹쳐 있었고, 지금도 겹쳐 있다.
		// Enter 는 최초 진입 1회만 호출되므로, 겹침이 유지되는 동안
		// 침투를 해소할 기회는 Stay 에서만 줄 수 있다. (예: 벽 방향 입력을 유지한 채 파고드는 경우)
		actor1->OnStay(actor2, result);
		actor2->OnStay(actor1, result);
	}
}

void CollisionManager::setIgnoreMask(ActorType A, ActorType B)
{
	// 항상 양방향으로 관리
	IGNORE_MASK[(int32)A][(int32)B] = true;
	IGNORE_MASK[(int32)A][(int32)A] = true;

	IGNORE_MASK[(int32)B][(int32)B] = true;
	IGNORE_MASK[(int32)B][(int32)A] = true;
}

void CollisionManager::checkCollision(Actor* actor)
{
	int32 gridSize = SceneManager::GetInstance().GetScene()->GetGridSize();

	// x, +- 1
	// y, +- 1
	Cell cell = Cell::ConvertToCell(actor->GetPos(), gridSize);

	// 여러 액터와 동시에 겹쳤을 때, 그중 가장 깊이 관통한(= 가장 가깝게 충돌한) 액터 하나만 기억해둔다.
	Actor* closestActor = nullptr;
	HitResult closestHit;

	// 인접한 셀을 순회한다. 9번
	for (int i = -1; i <= 1; ++i)
	{
		for (int j = -1; j <= 1; ++j)
		{
			Cell checkCell{ cell.iX + i, cell.iY + j };

			// 인접한 그리드 셀이 관리하고 있는 Actor를 전체다 순회하면서 충돌체크를 수행
			const GridInfo& gridInfo = SceneManager::GetInstance().GetScene()->GetGridInfo(checkCell);
			for (const auto& otherActor : gridInfo.actors)
			{
				// 같은 녀석은 건너띈다.
				if (actor == otherActor)
					continue;

				// 충돌체크를 안해도 되는 녀석은 건너띈다.
				if (IGNORE_MASK[(int32)actor->GetActorType()][(int32)otherActor->GetActorType()] == true)
					continue;

				if (actor->GetCollider() == nullptr)
					continue;
				if (otherActor->GetCollider() == nullptr)
					continue;

				// 충돌체크 검사
				// Circle vs Circle (1945) 항상되었는데,
				// Circle vs AABB
				// AABB vs Circle
				// AABB vs AABB
				// 방법 1)
				//dynamic_cast<> -> 성능상 매프레임 수행하는건 좋지않다.
				
				// 방법 2)
				//actor->GetCollider()->GetType() == circle ? -> static_cast<>
				//otherActor->GetCollider()->GetType() == aabb ? static_cast<>
				//if (actor->GetCollider()->GetType() == circle && 
				//	otherActor->GetCollider()->GetType() == circle)
				//{
				//	circle to circle
				//}
				//else if (actor->GetCollider()->GetType() == circle && 
				//	otherActor->GetCollider()->GetType() == aabb)
				//{
				//	circle to aabb
				//}
				//else if (actor->GetCollider()->GetType() == aabb &&
				//	otherActor->GetCollider()->GetType() == aabb)
				//{
				//	aabb to aabb
				//}

				// 방법 3) 방법2번의 if/else 구문이 싫다.
				// 타입을 테이블화 -> 테이블 함수를 호출 -> 디스패치 테이블 패턴?
				ColliderType aType = actor->GetCollider()->GetType();
				ColliderType bType = otherActor->GetCollider()->GetType();

				// 지금 수행하는 Hit 결과
				HitResult hit;

				// 테이블에 있는 충돌체크 함수를 통해서 충돌체크 여부를 판단한다.
				if(DISPATCH_TABLE[(int32)aType][(int32)bType](actor->GetCollider(), otherActor->GetCollider(), hit))
				{
					// 바로 이벤트를 발생시키지 않고, 지금까지 중 가장 깊이 겹친 후보만 갱신한다.
					if (closestActor == nullptr || hit.depth > closestHit.depth)
					{
						closestActor = otherActor;
						closestHit = hit;
					}

					//addOverlapState(actor, otherActor);
					//bullet->OnHit(enemy);
					//enemy->OnHit(bullet);
				}
			}
		}
	}

	if(closestActor != nullptr)
	{
		// 충돌되었다면, 양쪽 Actor에 누구와 충돌했는지 알려준다.
		// Enter, Stay, Exit
		addOverlapState(actor, closestActor, closestHit);
	}
}

void CollisionManager::drawGridLine(HDC hdc)
{
	int32 gridSize = SceneManager::GetInstance().GetScene()->GetGridSize();

	// 빨간색 그리드 배경 선
	{
		// 화면 크기와 그리드 크기 설정
		int32 width = GWinSizeX;
		int32 height = GWinSizeY;

		// 빨간색 펜 생성
		HPEN redPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
		HPEN oldPen = (HPEN)SelectObject(hdc, redPen);

		// 가로선 그리기
		for (int y = 0; y <= height; y += gridSize)
		{
			MoveToEx(hdc, 0, y, nullptr); // 시작점 설정
			LineTo(hdc, width, y);        // 끝점까지 선 그리기
		}

		// 세로선 그리기
		for (int x = 0; x <= width; x += gridSize)
		{
			MoveToEx(hdc, x, 0, nullptr); // 시작점 설정
			LineTo(hdc, x, height);       // 끝점까지 선 그리기
		}

		// 이전 펜 복원 및 새 펜 삭제
		SelectObject(hdc, oldPen);
		DeleteObject(redPen);
	}

	// 충돌 체크가 필요한 선만 그리기 : 진한 청록색
	{
		// 펜 생성
		HPEN myPen = CreatePen(PS_SOLID, 3, RGB(0, 255, 255));
		HPEN oldPen = (HPEN)SelectObject(hdc, myPen);

		for (auto actor : _collisionCheckList)
		{
			const Cell& cell = Cell::ConvertToCell(actor->GetPos(), gridSize);

			// 인접한 셀 모두 표시
			for (int32 i = -1; i < 2; ++i)
			{
				for (int32 j = -1; j < 2; ++j)
				{
					Cell checkCell{ cell.iX + i, cell.iY + j };

					// 사각형 그리기
					int32 x = checkCell.iX * gridSize;
					int32 y = checkCell.iY * gridSize;

					{
						MoveToEx(hdc, x, y, nullptr);
						LineTo(hdc, x + gridSize, y);
					}
					{
						MoveToEx(hdc, x + gridSize, y, nullptr);
						LineTo(hdc, x + gridSize, y + gridSize);
					}
					{
						MoveToEx(hdc, x + gridSize, y + gridSize, nullptr);
						LineTo(hdc, x, y + gridSize);
					}
					{
						MoveToEx(hdc, x, y + gridSize, nullptr);
						LineTo(hdc, x, y);
					}
				}
			}
		}

		// 이전 펜 복원 및 새 펜 삭제
		SelectObject(hdc, oldPen);
		DeleteObject(myPen);
	}
}
