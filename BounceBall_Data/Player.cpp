#include "pch.h"
#include "Player.h"
#include "SpriteRenderer.h"
#include "InputManager.h"
#include "Game.h"
#include "Scene.h"
#include "PlayerScene.h"
#include "SceneManager.h"

void Player::Init()
{
	SpriteAnimRenderer* sprite = AddComponent<SpriteAnimRenderer>();
	sprite->Init(L"Player");
	sprite->SetSize(PLAYER_SIZE, PLAYER_SIZE);
	sprite->SetFullFrame(false);

	_sprite = sprite;
	_destPos = GetPos();
	changeState(PlayerState::Idle);
}

void Player::Update(float deltaTime)
{
	Super::Update(deltaTime);

	switch (_state)
	{
	case PlayerState::Idle:     updateIdle(deltaTime);     break;
	case PlayerState::Move:     updateMove(deltaTime);     break;
	case PlayerState::FindPath: updateFindPath(deltaTime); break;
	}
}

void Player::Render(HDC hdc)
{
	Super::Render(hdc);
}

void Player::changeState(PlayerState s)
{
	_state = s;

	// player의 상태에 따라 재생중인 sprite Render의 animation 도 변경한다.
	switch (s)
	{
	case PlayerState::Idle:
		_sprite->ResetAnim(0, true, IDLE_ANIM_DURATION);
		break;
	case PlayerState::Move:
		_sprite->ResetAnim(1, false, MOVE_DURATION);
		break;
	case PlayerState::FindPath:
		_sprite->ResetAnim(1, true, MOVE_DURATION);
		break;
	}
}

void Player::updateIdle(float dt)
{
	Vector dir(0, 0);

	// 키입력 방향에 따라 셀을 이동한다.
	if (InputManager::GetInstance().GetButtonDown(KeyType::Left))
	{
		// 텍스처는 1개인데 좌우반전해서 표현해야해서 Flip 기능을 구현한다.
		dir = Vector(-BLOCK_SIZE, 0);
		_facingLeft = true;
		_sprite->SetFlipX(true);
	}
	else if (InputManager::GetInstance().GetButtonDown(KeyType::Right))
	{
		dir = Vector(BLOCK_SIZE, 0);
		_facingLeft = false;
		_sprite->SetFlipX(false);
	}
	else if (InputManager::GetInstance().GetButtonDown(KeyType::Up))
	{
		dir = Vector(0, -BLOCK_SIZE);
	}
	else if (InputManager::GetInstance().GetButtonDown(KeyType::Down))
	{
		dir = Vector(0, BLOCK_SIZE);
	}
	else if (InputManager::GetInstance().GetButtonDown(KeyType::F1))
	{
		SceneManager::GetInstance().ChangeScene(SceneType::Lobby);
	}

	// 키입력이 없다면 종료
	if (dir.x == 0.f && dir.y == 0.f)
	{
		return;
	}

	// 다음으로 이동해야할 좌표 계산
	Vector nextPos = GetPos() + dir;

	// 경계면 무시
	if (nextPos.x < 0 || nextPos.y < 0 ||
		nextPos.x + BLOCK_SIZE > GWinSizeX ||
		nextPos.y + BLOCK_SIZE > GWinSizeY)
		return;

	// 이동 가능한 셀인지 확인
	Cell currCell = Cell::ConvertToCell(GetPos(), BLOCK_SIZE);
	Cell nextCell = Cell::ConvertToCell(nextPos, BLOCK_SIZE);
	
	PlayerScene* playerScene = dynamic_cast<PlayerScene*>(SceneManager::GetInstance().GetScene());
	if(playerScene->CanMoveCell(nextCell) == false)
	{
		return;
	}

	_startRenderPos = GetPos();
	_destPos = nextCell.ConvertToCenterPos(BLOCK_SIZE);
	_moveTimer = 0;
	
	changeState(PlayerState::Move);
}

void Player::updateMove(float dt)
{
	_moveTimer += dt;
	float t = min(_moveTimer / MOVE_DURATION, 1.0f);

	Vector newPos = _startRenderPos + (_destPos - _startRenderPos) * t;
	SetPos(newPos);

	if (t >= 1.0f)
	{
		// 충분히 이동했다면 목표가되는 좌표로 설정한다.
		SetPos(_destPos);
		changeState(PlayerState::Idle);
	}
}

void Player::updateFindPath(float dt)
{
	_moveTimer += dt;
	float t = min(_moveTimer / MOVE_DURATION, 1.0f);
	
	Vector newPos = _startRenderPos + (_destPos - _startRenderPos) * t;
	SetPos(newPos);

	if (t < 1.0f)
		return;

	if (!moveToNextCell())
	{
		_path.clear();
		_pathIndex = 0;
		changeState(PlayerState::Idle);
	}
}

void Player::SetPath(const vector<Cell>& path)
{
	if (path.empty()) return;

	_path = path;
	_pathIndex = 0;

	if (moveToNextCell())
	{
		changeState(PlayerState::FindPath);
	}
}

bool Player::moveToNextCell()
{
	if (_pathIndex >= (int32)_path.size())
		return false;

	Cell nextCell = _path[_pathIndex];
	
	// 길찾기 셀의 중심 좌표를 물리적인 좌표로 지정한다.
	Vector nextPos = nextCell.ConvertToCenterPos(BLOCK_SIZE);

	// 갈수 없는 지역
	PlayerScene* playerScene = dynamic_cast<PlayerScene*>(SceneManager::GetInstance().GetScene());
	if (playerScene->CanMoveCell(nextCell) == false)
	{
		return false;
	}

	++_pathIndex;

	// 가고자 하는 방향에 따라 Sprite의 방향을 왼/오 변경해준다.
	if (nextPos.x < GetPos().x)     
	{
		_facingLeft = true;  _sprite->SetFlipX(true); 
	}
	else if (nextPos.x > GetPos().x) 
	{ 
		_facingLeft = false; _sprite->SetFlipX(false); 
	}

	_startRenderPos = GetPos();
	_destPos = nextPos;
	_moveTimer = 0;
	
	return true;
}
