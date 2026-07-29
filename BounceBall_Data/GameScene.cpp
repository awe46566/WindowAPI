#include "pch.h"
#include "GameScene.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "StageLoader.h"
#include "ResourceManager.h"
#include "Effect.h"
#include "SoundManager.h"
#include "UIImage.h"
#include "UIButton.h"

GameScene::GameScene()
{
	_sceneType = SceneType::Game;
}

void GameScene::Update(float deltaTime)
{
	Scene::Update(deltaTime);

	if (InputManager::GetInstance().GetButtonDown(KeyType::F1))
	{
		SceneManager::GetInstance().ChangeScene(SceneType::Lobby);
	}

	_nextLevelPopup.Update(deltaTime);
}

void GameScene::Render(HDC hdc)
{
	Scene::Render(hdc);

	{
		HFONT font = ResourceManager::GetInstance().GetFont(FontSize::FONT_30);
		HFONT oldFont = (HFONT)::SelectObject(hdc, font);

		wstring str = L"도전 횟수({0})";
		::TextOut(hdc, 5, 40, str.c_str(), static_cast<int32>(str.size()));

		::SelectObject(hdc, oldFont);
	}

	_nextLevelPopup.Render(hdc);
}

void GameScene::Dead(Vector pos)
{
	// 파티클 보여주고
	// 여기서 바로 파티클 재생시키면
	// post Action 에서 loadStage 하면서 모든 Actor 제거한다. -> 같이 파티클도 제거
	SoundManager::GetInstance().Play(L"DeadS");
	
	// 스테이지를 재로드할때, 바로 loadStage 호출하면 대참사.
	// 한프레임 뒤로 미룰꺼다..
	AddPostUpdateAction([this, pos]()
		{
			loadStage();

			// 죽음 파티클 재생
			Effect* effect = new Effect();
			effect->Init(L"DeadEffect");
			effect->SetPos(pos);
			AddActor(effect);
		});
}

void GameScene::AddStarCount(Vector pos)
{
	_curStarCount++;
	
	SoundManager::GetInstance().Play(L"Star");

	// 획득 파티클 재생
	Effect* effect = new Effect();
	effect->Init(L"EatStarEffect");
	effect->SetPos(pos);
	AddActor(effect);

	if (_maxStarCount <= _curStarCount)
	{
		// 별을 모두 획득하면 보이는 팝업 Show
		_nextLevelPopup.Show();
	}
}

void GameScene::createObjects()
{
	// 스테이지 로드
	loadStage();

	// 해당 UI 팝업은 처음에는 안보이고, 별을 모두 획득하면 보이는 팝업
	_nextLevelPopup.Hide();

	UIImage* title = new UIImage();
	title->Init(L"LevelComplete", Vector(GWinSizeX - 500, 180));
	_nextLevelPopup.AddWidget(title);

	UIButton* nextButton = new UIButton();
	nextButton->Init(L"NextStageButton", Vector(GWinSizeX - 500, 450));
	nextButton->SetOnClick([this]
		{
			// 다음 스테이지로 전환
			_curStarCount = 0;
			_currStage += 1;

			// Stage 다시 로드시, CollisionManager의 자료구조도 변경되니깐 
			// PostUpdate에 넣어야한다.
			//StageLoader::Load()
			//	Scene::RemoveAllActor()
			//	_actors.clear();	// 자료 구조 클리어
			//		CollisionManager::Clear(); // CollisionManager 자료구조 클리어
			AddPostUpdateAction([this]()
				{
					loadStage();

					// 다음 스테이지로 넘어갔으니 스테이지 전환 팝업 숨기기
					_nextLevelPopup.Hide();
				});

			//loadStage();
		});
	_nextLevelPopup.AddWidget(nextButton);
}

void GameScene::loadStage()
{
	wstring fileName = std::format(L"stage_{0}.json", _currStage);

	fs::path path = ResourceManager::GetInstance().GetResourcePath()
		/ L"Data/StageJson" / fileName;

	std::ifstream file(path);
	if (file.is_open())
	{
		// nlohmann
		json root = json::parse(file);

		// 처음에 첫 스테이지 -> 두번째 스테이지도 로드
		StageLoader loader;

		// 획득해야하는 별의 개수를 저장
		_maxStarCount = loader.Load(this, root);
	}
}