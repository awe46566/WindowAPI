#include "pch.h"
#include "StageLoader.h"
#include "Scene.h"
#include "Actor.h"
#include "EditorScene.h"
#include "Ball.h"
#include "PlayerScene.h"

int32 StageLoader::Load(Scene* scene, json root)
{
	int32 starCount = 0;

	// 기존에 추가된 actor 들이 있을것이다.
	// 모든 actor를 제거하고, 새로 json 으로부터 읽어서 객체를 생성한다.
	scene->RemoveAllActor();

	// ',' 기준으로 x,y 값 가져오기
	auto parsePosPair = [](const std::string& s, Actor* actor)
		{
			size_t comma = s.find(',');
			int32 x = std::stoi(s.substr(0, comma));
			int32 y = std::stoi(s.substr(comma + 1));

			actor->SetPos(Vector((float)x, (float)y));
		};

	// bg
	{
		Actor* actor = scene->CreateActor(ActorType::Background);
		scene->AddActor(actor);
	}

	if (root.contains("block"))
	{
		for (const json& block : root["block"])
		{
			Actor* actor = scene->CreateActor(ActorType::Block);

			// 공통으로 pos 문자열 읽고, SetPos() 해주는 람다식
			parsePosPair(block["pos"], actor);

			scene->AddActor(actor);
		}
	}

	if (root.contains("star"))
	{
		for (const json& star : root["star"])
		{
			++starCount;

			Actor* actor = scene->CreateActor(ActorType::Star);

			// 공통으로 pos 문자열 읽고, SetPos() 해주는 람다식
			parsePosPair(star["pos"], actor);

			scene->AddActor(actor);
		}
	}

	if (root.contains("ball"))
	{
		// PlayerScene일경우 Ball 대신 Player 생성
		PlayerScene* playerScene = dynamic_cast<PlayerScene*>(scene);
		ActorType type = playerScene ? ActorType::Player : ActorType::Ball;

		Actor* actor = scene->CreateActor(type);
		
		// 공통으로 pos 문자열 읽고, SetPos() 해주는 람다식
		parsePosPair(root["ball"]["pos"], actor);
		scene->AddActor(actor);

		EditorScene* editorScene = dynamic_cast<EditorScene*>(scene);
		if (editorScene)
		{
			Ball* ball = static_cast<Ball*>(actor);
			ball->SetEditorScene();
		}
	}

	// 해당스테이지에 추가된 별의 개수
	return starCount;
}