#include "pch.h"
#include "EditorScene.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "Ball.h"
#include "Star.h"
#include "Block.h"
#include "Game.h"
#include "StageLoader.h"

EditorScene::EditorScene()
{
	_sceneType = SceneType::Editor;
}

void EditorScene::Update(float deltaTime)
{
	Scene::Update(deltaTime);

	if (InputManager::GetInstance().GetButtonDown(KeyType::F1))
	{
		setPreviewType(ActorType::Block);
	}
	if (InputManager::GetInstance().GetButtonDown(KeyType::F2))
	{
		setPreviewType(ActorType::Ball);
	}
	if (InputManager::GetInstance().GetButtonDown(KeyType::F3))
	{
		setPreviewType(ActorType::Star);
	}

	// 좌클릭 -> 추가
	if (InputManager::GetInstance().GetButtonDown(KeyType::LeftMouse))
	{
		// 위치도 마우스 클릭한 좌표를, Grid의 중심좌표로 변신해서 넣어주자.
		Cell cell = Cell::ConvertToCell(InputManager::GetInstance().GetMousePos(), BLOCK_SIZE);

		// 동일한 위치의 Cell에 배치된 Actor들은 모두 삭제
		DeleteActorByCell(cell);

		// Ball 은 한개만 존재한다.
		if (_previewType == ActorType::Ball)
		{
			if (Actor* ballActor = FindActorByType(ActorType::Ball))
			{
				DeleteActor(ballActor);
			}
		}

		// 클릭하면, Scene에 배치하면 된다.
		// 미리보기용 객체를 Scene에 추가
		Actor* newActor = CreateActor(_previewType);
		AddActor(newActor);

		Vector centerPos = cellCenter(cell);
		newActor->SetPos(centerPos);

		if (_previewType == ActorType::Ball)
		{
			Ball* ball = static_cast<Ball*>(newActor);
			ball->SetEditorScene();
		}
	}

	// 우클릭 -> 삭제
	if (InputManager::GetInstance().GetButtonDown(KeyType::RightMouse))
	{
		// 삭제하고자 하는 cell
		Cell cell = Cell::ConvertToCell(InputManager::GetInstance().GetMousePos(), BLOCK_SIZE);

		// 해당 cell 에 위치한 Actor 를찾아서
		// 삭제 요청하면 된다.
		DeleteActorByCell(cell);
	}

	// Save
	if (InputManager::GetInstance().GetButtonDown(KeyType::S))
	{
		saveStage();
	}
	// Load
	if (InputManager::GetInstance().GetButtonDown(KeyType::L))
	{
		// load 하는공간이 두군데. 
		// EditorScene	-> Load
		// GameScene	-> Load (현재 스테이지에 맞게)
		loadStage();
	}

	// 미리보기용 객체는 항상 마우스를 따라다니게 구현한다.
	if (_preview)
	{
		_preview->SetPos(InputManager::GetInstance().GetMousePos(), false);
	}
}

void EditorScene::Render(HDC hdc)
{
	Scene::Render(hdc);
	
	for (int y = 0; y <= GWinSizeY; y += BLOCK_SIZE)
	{
		MoveToEx(hdc, 0, y, nullptr);
		LineTo(hdc, GWinSizeX, y);
	}

	for (int x = 0; x <= GWinSizeX; x += BLOCK_SIZE)
	{
		MoveToEx(hdc, x, 0, nullptr);
		LineTo(hdc, x, GWinSizeY);
	}

	// _preview 객체는 Scene에서 관리하는게 아니라서, EditorScene 직접 Render 호출
	if (_preview)
	{
		_preview->Render(hdc);
	}

	{
		wstring str = L"EditorScene";
		::TextOut(hdc, 5, 40, str.c_str(), static_cast<int32>(str.size()));
	}
}

void EditorScene::createObjects()
{
	_preview = CreateActor(_previewType);
}

Vector EditorScene::cellCenter(Cell cell)
{
	return cell.ConvertToCenterPos(BLOCK_SIZE);
}

void EditorScene::saveStage()
{
	OPENFILENAME ofn;
	wchar_t szFileName[MAX_PATH] = L"";

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = Game::GetInstance().GetHwnd();
	ofn.lpstrFilter = L"스테이지 파일 (*.json)\0*.json\0모든 파일 (*.*)\0*.*\0";
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	ofn.lpstrDefExt = L"json";

	if (GetSaveFileName(&ofn))
	{
		// 파일 이름이 선택되었으면 저장
		std::wstring fileName = szFileName;

		std::wofstream file(fileName);
		if (file.is_open())
		{
			// 데이터 저장
			json root;
			root["star"] = json::array();
			root["block"] = json::array();
			root["item"] = json::array();

			// 전체 Actor 순회하면서 json으로 저장
			for (auto actor : _actors)
			{
				// json { "pos" = "x,y" }
				json entry;
				entry["pos"] = std::format("{},{}", (int32)actor->GetPos().x, (int32)actor->GetPos().y);

				switch (actor->GetActorType())
				{
				case ActorType::Star:  root["star"].push_back(entry);  break;
				case ActorType::Block: root["block"].push_back(entry); break;
				case ActorType::Ball:  root["ball"] = entry;           break;
				default: break;
				}
			}

			// json 객체를 보기 좋게 들여쓰기된 JSON 텍스트로 변환해서 파일에 그대로 써넣는다
			file << root.dump(4).c_str();
			
			file.close();
			MessageBox(Game::GetInstance().GetHwnd(), L"저장이 완료되었습니다.", L"저장 완료", MB_OK | MB_ICONINFORMATION);
		}
		else {
			MessageBox(Game::GetInstance().GetHwnd(), L"파일을 저장할 수 없습니다.", L"에러", MB_OK | MB_ICONERROR);
		}
	}
}

void EditorScene::loadStage()
{
	OPENFILENAME ofn;
	wchar_t szFileName[MAX_PATH] = L"";

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = Game::GetInstance().GetHwnd();
	ofn.lpstrFilter = L"스테이지 파일 (*.json)\0*.json\0모든 파일 (*.*)\0*.*\0";
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	ofn.lpstrDefExt = L"json";

	if (GetOpenFileName(&ofn))
	{
		// 파일 이름이 선택되었으면 로드
		std::wstring fileName = szFileName;

		std::ifstream file(fileName);
		if (file.is_open())
		{
			// nlohmann
			json root = json::parse(file);

			// 로드를 담당하는 클래스를 통해서 모든 Scene 구성한다.
			StageLoader loader;
			loader.Load(this, root);

			file.close();
			MessageBox(Game::GetInstance().GetHwnd(), L"로드 완료되었습니다.", L"로드 완료", MB_OK | MB_ICONINFORMATION);
		}
		else {
			MessageBox(Game::GetInstance().GetHwnd(), L"파일을 로드할 수 없습니다.", L"에러", MB_OK | MB_ICONERROR);
		}
	}
}

void EditorScene::setPreviewType(ActorType type)
{
	if (_previewType == type && _preview)
		return;

	delete _preview;
	_previewType = type;
	_preview = CreateActor(type);
	_preview->SetPos(Vector(InputManager::GetInstance().GetMousePos()), false);
}
