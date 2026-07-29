#include "pch.h"
#include "Scene.h"
#include "Background.h"
#include "ResourceManager.h"
#include "TimeManager.h"
#include "CollisionManager.h"
#include "Effect.h"
#include "DataManager.h"
#include "ResourceData.h"
#include "Ball.h"
#include "Star.h"
#include "Block.h"
#include "Player.h"

// 생성자/소멸자를 cpp 작성하면, Scene의 인스턴스화는 cpp에서 일어남.
// ObjectPool<T> (vector<T>) 값 자체를 가지고 있는 풀을 생성하는것도,
// cpp에서 인스턴스화할때 생성됨.
// 이때는 Bullet/Enemy #include 완료 상태
Scene::Scene() 
{
}
Scene::~Scene()
{
}

void Scene::Init()
{
	// Grid 미리 생성
	_gridCountX = (int32)GWinSizeX / _gridSize;
	_gridCountY = (int32)GWinSizeY / _gridSize;

	int32 totalGridCount = _gridCountX * _gridCountY;
	_grid.resize(totalGridCount);

	// Scene -> LobbyScene, GameScene, EditScene
	// Scene에 필요한 리소스 로드
	loadResources();

	// Scene에 필요한 객체 생성
	createObjects();


	// ObjectPool 미리 생성
	//for (int32 i = 0; i < 100; ++i)
	//{
	//	// 이 방식은, 운영체제가 알아서 메모리를 할당해주기때문에
	//	// 연속메모리를 줄수도 있고, 아닐수도 있고.
	//	// 캐시 히트율이 좋을수도 있고, 안좋을수도 있다.
	//	_bulletList.push_back(new Bullet());	
	//}

	// vector 자체 순회는 캐시 히트가 좋지만,
	//for (auto bullet : _bulletList)
	//{
	//	bullet->GetPos();	// 직접 Bullet 을 찾아가서 정보를 읽어야 한다면, 캐시 미스가 발생할 확률이 있다.
	//}
}

void Scene::Cleanup()
{
	// 씬에 등장하는 모든 객체들의 delete 담당
	for (auto iter : _actors)
	{
		// Scene이 new 한 객체는 delete 해도 된다.
		if (iter->GetPool() == nullptr)
		{
			delete iter;
		}
	}
	_actors.clear();
}

void Scene::Update(float deltaTime)
{
	for (auto actor : _actors)
	{
		actor->Update(deltaTime);

			//Ball::Update()
			//	GameScene::Dead()
			//		StageLoader::Load()
			//			Scene::RemoveAllActor()
			//				_actors.clear();	// 자료 구조 클리어
	}

	// 삭제가 필요한 애들은 삭제
	// 1번 방식으로 '삭제 여부' 걸러도 되고,
	// 2번 방식으로 '삭제 여부' 걸러도 되고
	std::erase_if(_actors, [this](Actor* actor)
		{
			// 1번 방식은, _actor에서 제거는 되는데,
			// 제거되기전에 아래 delete + removeActor 함수 호출해야해서
			// 2번 방식으로 별도 리스트를 관리하는게 좋겟다.
			//return actor->GetPendingKill(); 
			return _reservedRemove.contains(actor);
		});

	// 실제 메모리 해제 까먹었따.
	for (auto deleteActor : _reservedRemove)
	{
		// 삭제되는 Actor
		removeActor(deleteActor);

		// 해당 Actor가 풀에서 태어난 경우에는 반환
		if (deleteActor->GetPool())
		{
			// 해당 풀에다가 반환
			deleteActor->GetPool()->Return(deleteActor);
		}
		else
		{
			// new 태어난 경우는 delete
			delete deleteActor;
		}
	}

	// 추가가 필요한 애들은 추가
	// 1번 방식 : 매번 push_back 할때마다 비용 지불
	//for (auto iter : _reservedAdd)
	//{
	//	// vector
	//	// capacity, size
	//	// 새로 원소를 집어넣을떄 capacity 부족시, 메모리 추가 할당
	//	_actors.push_back(iter);
	//}

	// 2번 방식 :
	// 여긴, reserverdAdd 에 10개가 있을경우
	// vector를 한번에 10개 늘리고 복사해와서, 재할당이 1번 일어난다.
	//_actors.insert(_actors.end(), _reservedAdd.begin(), _reservedAdd.end());

	//-> 미리 한번만 할당해놓고, 복사하기
	_actors.reserve(_actors.size() + _reservedAdd.size()); // 개수X, Capacity(메모리)
	for (Actor* actor : _reservedAdd)
	{
		// 추가되는 Actor
		registerActor(actor);		
		_actors.push_back(actor);	// reserve() 함수로 미리 capacity 확보해뒀다.
	}

	// 지연리스트 초기화
	_reservedAdd.clear();
	_reservedRemove.clear();

	// 그리드 갱신 : 초기화 -> 재갱신 이방식이 마음에 안든다면,
	// Actor의 위치가 변경될때마다 Grid 의 위치를 갱신해주는 방식을 하면 된다.
	// 즉, 아래 코드는 다 사라지고 Actor가 Scene에게 요청을 해서, Grid 갱신한다.
	{
		// 모든 Actor의 최신화된 좌표 기준으로 Grid 갱신
		// 이전프레임에 있었던 Grid 정보는 초기화
		//for (GridInfo& grid : _grid)
		//{
		//	grid.actors.clear();
		//}

		// 1945 게임 특성상 매프레임 위치 변경이 있으니깐, 
		// 그냥 전체 순회하면서 Grid 등록을 해준다.
		// 전체 순회니깐, 어차피 또 성능적인 측면의 이점이 없는거 아닌가요.
		// O(N*M) -> O(N)
		//for (auto actor : _actors)
		//{
		//	updateGrid(actor);
		//}
	}


	// 바운스볼 게임 특성상 : Ball 만 위치이동이 있다.
	// Ball 이동할때마다 Grid 를 갱신하도록 수정
}

void Scene::Render(HDC hdc)
{
	// 명확한 렌더링 순서를 지키기 위해 별도의 리스트 순서대로 그린다.
	for (auto list : _renderList)
	{
		for (auto actor : list)
		{
			actor->Render(hdc);
		}
	}

	//for (auto actor : _actors)
	//{
	//	actor->Render(hdc);
	//}
}

void Scene::PostUpdate()
{
	// 후처리 액션 수행 (std::function)
	for (auto& action : _postUpdateActions)
	{
		// 함수포인터 호출 : 결합도 느슨해진다.
		action();
	}
	_postUpdateActions.clear();
}

void Scene::AddPostUpdateAction(std::function<void()> action)
{
	_postUpdateActions.push_back(action);
}

void Scene::AddActor(class Actor* actor)
{
	// 즉시 추가안하고, 안전하게 모든 update다 돌고나서 추가하자.
	_reservedAdd.push_back(actor);
}

void Scene::DeleteActor(Actor* actor)
{
	// 여기에서 즉시 삭제하지 않는다.
	// vector 빼고, delete 해주고.
	//	_actors.erase

	// 지연 삭제
	// 추가적인 리스트에 넣고, 나중에 한번에 삭제
	_reservedRemove.insert(actor);
}

bool Scene::isValidCell(Cell cell) const
{
	return cell.iX >= 0 && cell.iX < _gridCountX
		&& cell.iY >= 0 && cell.iY < _gridCountY;
}

void Scene::DeleteActorByCell(Cell cell)
{
	if (false == isValidCell(cell))
		return;

	int32 index = cell.iY * _gridCountX + cell.iX;
	for (Actor* actor : _grid[index].actors)
	{
		DeleteActor(actor);
	}
}

Actor* Scene::FindActorByType(ActorType type) const
{
	for (Actor* actor : _actors)
	{
		if (actor->GetActorType() == type)
		{
			return actor;
		}
	}

	return nullptr;
}

void Scene::RemoveAllActor()
{
	// 그리드
	for (auto& grid : _grid)
	{
		grid.actors.clear();
	}

	// 예약된 추가 리스트
	for (auto actor : _reservedAdd)
	{
		delete actor;
	}
	_reservedAdd.clear();

	// 예약된 삭제 리스트
	_reservedRemove.clear();

	// 현재 추가된 리스트
	for (auto iter : _actors)
	{
		delete iter;
	}
	_actors.clear();

	for (auto& iter : _renderList)
	{
		iter.clear();
	}

	// 충돌 매니저도 clear
	CollisionManager::GetInstance().Clear();
}

void Scene::CreateEffect(Vector pos)
{
	Effect* effect = new Effect();
	effect->Init(L"Effect");
	effect->SetPos(pos);

	_reservedAdd.push_back(effect);
}


const vector<Actor*>& Scene::GetRenderList(RenderLayer layer) const
{
	if ((int32)layer < 0 || layer >= RenderLayer::Count)
	{
		static vector<Actor*> emptyList;
		return emptyList;
	}
	return _renderList[(int32)layer];
}

void Scene::loadResources()
{
	// DataManager에 가서 GameScene에 필요한 모든 데이터를 다 로드해달라고 요청
	ResourceData* data = DataManager::GetInstance().FindData<ResourceData>(L"ResourceData");
	if (data)
	{
		for (auto iter : data->_soundData)
		{
			const ResourceData::SoundItem& item = iter.second;
			ResourceManager::GetInstance().LoadSound(
				iter.first, item.fileName);
		}

		// 새로운 텍스처 로드가 필요하면, json에 추가하면 끝!
		for (auto iter : data->_sceneData[(int32)_sceneType])
		{
			const ResourceData::Item& item = iter.second;
			ResourceManager::GetInstance().LoadTexture(
				iter.first, 
				item.fileName, 
				item.transparent,
				item.countY,
				item.countX,
				item.dur,
				item.flip);
		}
	}


	/*
	// 실제 텍스처 로드 요청
	ResourceManager::GetInstance().LoadTexture(L"Player", L"Player.bmp", RGB(252, 0, 255));
	ResourceManager::GetInstance().LoadTexture(L"BG", L"BG.bmp", -1);
	ResourceManager::GetInstance().LoadTexture(L"Enemy1", L"Enemy1.bmp", RGB(255, 0, 255));
	ResourceManager::GetInstance().LoadTexture(L"Enemy2", L"Enemy2.bmp", RGB(255, 0, 255));
	ResourceManager::GetInstance().LoadTexture(L"Enemy3", L"Enemy3.bmp", RGB(255, 0, 255));
	ResourceManager::GetInstance().LoadTexture(L"Enemy4", L"Enemy4.bmp", RGB(255, 0, 255));
	ResourceManager::GetInstance().LoadTexture(L"EnemyBullet", L"EnemyBullet.bmp", RGB(0, 0, 0), 1, 5);
	ResourceManager::GetInstance().LoadTexture(L"Effect", L"explosion.bmp", RGB(0, 0, 0), 2, 6, 2.0f);
	ResourceManager::GetInstance().LoadTexture(L"Item", L"GoldTresureClosed.bmp", RGB(255, 0, 255));
	ResourceManager::GetInstance().LoadTexture(L"PlayerBullet", L"PlayerBullet.bmp", RGB(252, 0, 255));
	ResourceManager::GetInstance().LoadTexture(L"UI_HP", L"PlayerHP.bmp", RGB(255, 0, 255));
	ResourceManager::GetInstance().LoadTexture(L"Stage_2", L"Stage_2.bmp", -1);
	*/
}

void Scene::createObjects()
{

}

// Scene에 등록되는 Actor들이 모두 해야할일
void Scene::registerActor(Actor* actor)
{
	if (nullptr == actor)
		return;

	if (actor->GetRenderLayer() >= RenderLayer::Count)
		return;

	// 렌더링 순서에 맞게 리스트 갱신
	_renderList[(int32)actor->GetRenderLayer()].push_back(actor);

	// 충돌체크가 필요하다면, 충돌체크 등록
	if (actor->GetCollider())
	{
		CollisionManager::GetInstance().AddActor(actor);
	}
}

// Scene에 제거되는 Actor들이 모두 해야할일
void Scene::removeActor(Actor* actor)
{
	if (nullptr == actor)
		return;

	if (actor->GetRenderLayer() >= RenderLayer::Count)
		return;

	// RenderList vector에서 찾아서 제거
	std::erase_if(_renderList[(int32)actor->GetRenderLayer()],
		[actor](Actor* iter)
		{
			return iter == actor;
		});

	// grid에 남아있는것도 제거
	for (GridInfo& info : _grid)
	{
		std::erase(info.actors, actor);
	}

	// 삭제될 Actor포인터를 CollisionManager 에서 Enter/Exit 비교를 위해
	// 포인터를 저장하고 있기 때문에, 충돌체크에서도 빼자.
	if (actor->GetCollider())
	{
		CollisionManager::GetInstance().RemoveActor(actor);
	}
}


void Scene::UpdateGrid(Actor* actor, Vector prevPos)
{
	// Scene 관리하고 있는 객체일경우에만 grid 갱신
	//auto find = std::find_if(_actors.begin(), _actors.end(), [actor](Actor* iter) { return actor == iter; });
	//if (find == _actors.end())
	//{
	//	return;
	//}

	Cell prevCell = Cell::ConvertToCell(prevPos, _gridSize);
	Cell cell = Cell::ConvertToCell(actor->GetPos(), _gridSize);

	// 즉, prevCell == cell 같다면, 동일한 그리드에 소속되었으니 굳이 갱신X
	if (prevCell.iX == cell.iX && prevCell.iY == cell.iY)
	{
		return; // 갱신하지 않고 넘어가자.
	}

	// 이전 grid에서는 제거.
	if (isValidCell(prevCell))
	{
		int32 prevIndex = prevCell.iY * _gridCountX + prevCell.iX;
		std::erase(_grid[prevIndex].actors, actor);
	}

	// 새로운 grid에 추가
	if (isValidCell(cell))
	{
		int32 newIndex = cell.iY * _gridCountX + cell.iX;
		_grid[newIndex].actors.push_back(actor);
	}
}

const GridInfo& Scene::GetGridInfo(const Cell& cell)
{
	// per-axis bounds check to prevent index wraparound
	static GridInfo emptyGridInfo{};
	if (cell.iX < 0 || cell.iX >= _gridCountX || cell.iY < 0 || cell.iY >= _gridCountY)
		return emptyGridInfo;

	int32 index = cell.iY * _gridCountX + cell.iX;
	if (index >= 0 && index < _grid.size())
	{
		GridInfo& gridInfo = _grid[index];
		return gridInfo;
	}

	// null object 패턴.
	return emptyGridInfo;
}

Actor* Scene::CreateActor(ActorType type)
{
	switch (type)
	{
	case ActorType::Background:
	{
		Background* bg = new Background();
		bg->Init();
		return bg;
	}
	case ActorType::Block:
	{
		Block* block = new Block();
		block->Init();
		return block;
	}
	case ActorType::Ball:
	{
		Ball* ball = new Ball();
		ball->Init();
		return ball;
	}
	case ActorType::Star:
	{
		Star* star = new Star();
		star->Init();
		return star;
	}
	case ActorType::Player:
	{
		Player* player = new Player();
		player->Init();
		return player;
	}
	}

	return nullptr;
}