#include "pch.h"
#include "PlayerScene.h"
#include "Actor.h"
#include "Player.h"
#include "ResourceManager.h"
#include "InputManager.h"
#include "StageLoader.h"
#include "Texture.h"
#include "ResourceData.h"
#include "DataManager.h"

PlayerScene::PlayerScene()
{
	_sceneType = SceneType::AStar;
}

void PlayerScene::Update(float deltaTime)
{
	Super::Update(deltaTime);
	_debugAStar.Update(deltaTime);

	if (InputManager::GetInstance().GetButtonDown(KeyType::LeftMouse))
	{
		POINT mousePos = InputManager::GetInstance().GetMousePos();
		int32 cellX = mousePos.x / BLOCK_SIZE;
		int32 cellY = mousePos.y / BLOCK_SIZE;
		Cell clickedCell{ cellX, cellY };

		// 클릭한 위치를 Selector Icon 표시
		//if (CanMoveCell(clickedCell))
		{
			_selectorPos = Vector(
				(float)cellX * BLOCK_SIZE + BLOCK_SIZE / 2,
				(float)cellY * BLOCK_SIZE + BLOCK_SIZE / 2);
			_showSelector = true;

			Player* player = static_cast<Player*>(FindActorByType(ActorType::Player));
			if (player != nullptr)
			{
				Cell playerCell = Cell::ConvertToCell(player->GetPos(), BLOCK_SIZE);
				vector<Cell> path;
				FindPath(playerCell, clickedCell, path);
				if (!path.empty())
					player->SetPath(path);
			}
		}
	}
}

void PlayerScene::Render(HDC hdc)
{
	Super::Render(hdc);
	_debugAStar.Render(hdc);

	if (_showSelector)
	{
		if (_selectorTexture)
			_selectorTexture->Render(hdc, _selectorPos, Vector(0, 0));
	}
}

void PlayerScene::createObjects()
{
	_selectorTexture = ResourceManager::GetInstance().GetTexture(L"TileSelector");

	// 스테이지 로드
	loadStage();
}

void PlayerScene::loadStage()
{
	wstring fileName = L"a_star.json";

	fs::path path = ResourceManager::GetInstance().GetResourcePath()
		/ L"Data/StageJson" / fileName;

	std::ifstream file(path);
	if (file.is_open())
	{
		// nlohmann
		json root = json::parse(file);

		StageLoader loader;
		loader.Load(this, root);
	}
}

// 맨해튼 거리 휴리스틱
int Heuristic(Cell curr, Cell end)
{
	// 목적지까지의 거리 비교
	// 한셀당 이동하는 누적비용이 g += 10; 이라서
	// 휴리스틱 비용이 한 셀당 1씩 계산되면, 휴리스틱 비용이 흐려져서
	// * 10을 해준다.
	return (abs(end.iX - curr.iX) + abs(end.iY - curr.iY)) * 10;
}

bool PlayerScene::CanMoveCell(const Cell& cell)
{
	const GridInfo& gridInfo = GetGridInfo(cell);
	for (Actor* a : gridInfo.actors)
	{
		if (a->GetActorType() == ActorType::Block)
			return false;
	}
	return true;
}

// 길찾기 수행하는 로직
// findPath : 플레이어가 이동해야하는 Cell 을 넘겨준다.
// 최대로 탐색할 범위 : maxDepth, 너무 멀리있는 셀은 빙빙 돌아가야하니 굳이 탐색할 필요가 없다.
bool PlayerScene::FindPath(Cell start, Cell end, vector<Cell>& findPath, int32 maxDepth)
{
	findPath.clear();

	// 너무 멀면 무시
	int32 depth = start.DeltaLength(end);
	if (depth >= maxDepth)
		return false;

	// 같은 지점도 무시
	if (start == end)
		return false;

	// 큰수부터 거꾸로 뽑아가야하니깐, 음수를 넣어도 되고, stl의 grater를 넣어도 된다.
	priority_queue<PQNode, vector<PQNode>, greater<PQNode>> pq;
	_debugAStar.best.clear();
	_debugAStar.parent.clear();
	_debugAStar.closedList.clear();
	_debugAStar.openList.clear();

	// 최적의 노드
	// 방문했던 부모의 개념을 추가
	// g는 출발지에서 해당 셀까지 실제 이동 비용,
	// h는 목적지까지 추정 거리이므로
	// 초기값 설정
	{
		// 셀의 거리를 먼저 측정. 이것이 곧 비용이 된다.
		int g = 0;
		int h = Heuristic(start, end);
		int f = g + h;

		// 시작지점부터 측정
		pq.push(PQNode(g, h, start));
		_debugAStar.best[start] = f;
		_debugAStar.parent[start] = start;
		_debugAStar.openList[start] = PQNode(g, h, start);
	}

	int32 dirOrder[] = { DirType::DIR_RIGHT, DirType::DIR_DOWN, DirType::DIR_LEFT, DirType::DIR_UP };
	bool found = false;
	// 필요한 노드를 모두 순회했는지 확인
	while (pq.empty() == false)
	{
		// 인접한 노드를 방문해서, 제일 좋은 후보를 찾는다.
		PQNode node = pq.top();
		pq.pop();

		// 방문 완료했는지 확인
		if (_debugAStar.closedList.contains(node.pos))
		{
			continue;
		}

		// 지금보다 더 좋은경로를 이미 찾았다.
		if (_debugAStar.best[node.pos] < node.g + node.h)
		{
			continue;
		}

		// 목적지에 도착했으면 종료
		if (node.pos == end)
		{
			found = true;
			break;
		}

		_debugAStar.closedList.insert(node.pos);

		// 상하좌우. 인접한 노드를 방문해서 더 좋은 비용의 노드가 있는지 확인한다.
		for (int32 dir = 0; dir < 4; ++dir)
		{
			Cell nextCell = node.pos.NextCell((DirType)dirOrder[dir]);

			// 인접한 셀이 갈수 없는 영역이면 무시
			if (CanMoveCell(nextCell) == false)
			{
				continue;
			}

			// 방문해야하는 셀이 시적점과 너무 멀면 무시
			int32 depth = nextCell.DeltaLength(start);
			if (depth >= maxDepth)
				continue;

			if (_debugAStar.closedList.contains(nextCell))
			{
				continue;
			}

			// 해당 점수가 정말 최선인지 판단, 뒤늦게 최선의 경로가 발생할수도 있으니
			int g = node.g + 10;
			int h = Heuristic(nextCell, end);
			int f = g + h;

			// 처음 방문하는 노드가 아니라면, 비용 비교
			if (_debugAStar.best.find(nextCell) != _debugAStar.best.end())
			{
				// 다른 경로에서 더 빠른길을 찾았으면 스킵한다.
				if (_debugAStar.best[nextCell] <= f)
				{
					continue;
				}
			}

			// 예약을 진행
			pq.push(PQNode(g, h, nextCell));

			_debugAStar.best[nextCell] = f;
			_debugAStar.parent[nextCell] = node.pos;
			_debugAStar.openList[nextCell] = PQNode(g, h, nextCell);
		}
	}

	Cell newEnd = end;

	// end까지 경로가 없으면, 
	// h(목적지 거리) 최소 → 동률이면 g(누적 이동비용)
	// 목적지를 가장 가깝게 갈수있는 셀로 대체
	if (!found && !_debugAStar.closedList.empty())
	{
		int32 minH = INT32_MAX;
		int32 minG = INT32_MAX;
		for (const Cell& c : _debugAStar.closedList)
		{
			if (c == start)
				continue;
			int32 h = Heuristic(c, end);
			int32 g = _debugAStar.best[c] - h;
			if (h < minH || (h == minH && g < minG))
			{
				minH = h;
				minG = g;
				newEnd = c;
			}
		}
	}

	findPath.clear();
	Cell pos = newEnd;

	// 방문했던 리스트를 부모 노드로 찾아가면서 실제 path에 넣어준다.
	int32 safetyCount = 0;
	while (safetyCount++ < maxDepth * 2)
	{
		if (pos == start)
			break;

		auto it = _debugAStar.parent.find(pos);
		if (it == _debugAStar.parent.end())
			break;

		findPath.push_back(pos);
		pos = it->second;
	}

	std::reverse(findPath.begin(), findPath.end());
	return found;
}
