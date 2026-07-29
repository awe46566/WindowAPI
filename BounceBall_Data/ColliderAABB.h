#pragma once

#include "Collider.h"

class ColliderAABB : public Collider
{
	using Super = Collider;

public:
	void Init(class Actor* owner, int32 width, int32 height);

	// 소유자 위치(중심) 기준의 AABB 영역
	Vector GetMin();
	Vector GetMax();
	virtual ColliderType GetType() override { return ColliderType::AABB; }

public:
	int32		_width = 0;		// 가로 크기
	int32		_height = 0;	// 세로 크기
};

