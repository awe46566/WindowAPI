#pragma once

#include "ObjectPool.h"

//#include "Enemy.h"
//#include "Bullet.h"
// C++17
class Enemy;
class Bullet;

// 게임화면에 등장하는 모든 오브젝트를 관리
class Scene
{
public:
	// vector<T> 풀에서 사용하는 Enemy,Bullet 값자체를 전방선언으로 해결하기 위해
	// Scene의 생성자와 소멸자는 cpp 쪽에 구현을 해야한다.
	Scene();
	~Scene();

	void Init();
	void Cleanup();

	virtual void Update(float deltaTime);
	virtual void Render(HDC hdc);
	
	// 모든 업데이트가 끝나고 호출되는 함수
	void PostUpdate();
	void AddPostUpdateAction(std::function<void()> action);

	SceneType GetSceneType() { return _sceneType; }

	// 씬에 관리되는 Actor 추가
	void AddActor(class Actor* actor);

	// 씬에서 관리되는 Actor중에 하나 삭제해달라고 요청
	void DeleteActor(class Actor* actor);
	void DeleteActorByCell(Cell cell);
	
	Actor* FindActorByType(ActorType type) const;

	// 전부다 지우기
	void RemoveAllActor();

	void CreateEffect(Vector pos);
	Actor* CreateActor(ActorType type);

public:
	const vector<Actor*>& GetRenderList(RenderLayer layer) const;
	const GridInfo& GetGridInfo(const Cell& cell);
	int32 GetGridSize() const { return _gridSize; }

	void UpdateGrid(Actor* actor, Vector prevPos);

protected:
	virtual void loadResources();
	virtual void createObjects();

	// actor List / render List 의 동기화를 맞춰주기 위해서, 항상 호출되는 함수
	void registerActor(Actor* actor);
	void removeActor(Actor* actor);

	bool isValidCell(Cell cell) const;

protected:
	// 씬에 등장하는 모든 객체는 Actor로부터 파생된 클래스다.
	// 모든 클래스를 관리하는 공통 자료구조를 선언
	vector<Actor*> _actors;	 // 여기가 진짜 Update,Render하는 객체들

	// 렌더링 순서를 위한 list
	vector<Actor*> _renderList[(int32)RenderLayer::Count];
	
	// 지연 시스템
	// 이번 프레임에 추가되어야 하는 Actor들
	// add에 두번요청 들어올일이 없을것 같아서, vector 처리
	vector<Actor*>		_reservedAdd;	

	// 제거 요청을 중복처리하지 않기 위해, set 자료구조
	unordered_set<Actor*>	 _reservedRemove;		// vector vs map

	//class Background* _bg = nullptr;
	//class Player* _player = nullptr;
	//vector<class Enemy*> _enemies;
	//vector<class Bullet*> _bullets;

	// 공간 분할 (Grid)
	int32 _gridSize = BLOCK_SIZE;	// 유동적으로 수정하면 된다.
	int32 _gridCountX = 0;
	int32 _gridCountY = 0;

	// 하나의 그리드에 있는 Actor 관리
	vector<GridInfo> _grid;

	//vector<Actor*> _test1;    //1 : 메모리 연속X  / [0x01][0x02][0x10][][][]
	//vector<Actor> _test2;		//2 : 메모리 연속	//[Actor][Actor][Actor][Actor][Actor][Actor]
	
	// 메모리가 연속적이지 않아서, 2차원 vector (X) -> 1차원 vector (cell(x,y))
	//vector<vector<GridInfo>> _grid;	// 2차원 배열, [y][x]

	// 한프레임 뒤로 미뤄서 특정 함수를 호출해주는 기능
	std::vector<std::function<void()>> _postUpdateActions;

	// 본인의 타입
	SceneType _sceneType = SceneType::Max;
};


