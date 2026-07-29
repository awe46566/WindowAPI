#include "pch.h"
#include "Star.h"
#include "ImageRenderer.h"
#include "ColliderAABB.h"
#include "SceneManager.h"
#include "GameScene.h"
#include "Ball.h"

void Star::Init()
{
	ImageRenderer* renderer = AddComponent<ImageRenderer>();
	renderer->Init(L"Star");
	renderer->SetSize(STAR_SIZE, STAR_SIZE);

	// AABB 충돌체 추가
	ColliderAABB* collider = AddComponent<ColliderAABB>();
	collider->Init(this, STAR_SIZE, STAR_SIZE);
}

void Star::OnPickedUp(Ball* ball)
{
	// 별을 획득하는 처리
	GameScene* gameScene = dynamic_cast<GameScene*>(SceneManager::GetInstance().GetScene());
	gameScene->AddStarCount(ball->GetPos());

	// 별을 삭제해주세요.
	Destroy();
}
