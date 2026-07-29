#include "pch.h"
#include "Block.h"
#include "ImageRenderer.h"
#include "ColliderAABB.h"

void Block::Init()
{
	ImageRenderer* renderer = AddComponent<ImageRenderer>();
	renderer->Init(L"Block");
	renderer->SetSize(BLOCK_SIZE, BLOCK_SIZE);

	// AABB 충돌체 추가
	ColliderAABB* collider = AddComponent<ColliderAABB>();
	collider->Init(this, BLOCK_SIZE, BLOCK_SIZE);
}
