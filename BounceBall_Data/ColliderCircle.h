#pragma once

#include "Collider.h"

// A		vs	B : 충돌체크
// Circle	vs	Circle   -> CircleToCircle
// Circle	vs	AABB     -> CircleToAABB
// AABB		vs	Circle	 -> CircleToAABB
// AABB		vs	AABB	 -> AABBToAABB
// Circle	vs	OBB		 -> CircleToOBB


class ColliderCircle : public Collider
{
	using Super = Collider;
public:
	void Init(class Actor* owner, int radius);
	virtual void Render(HDC hdc, Vector pos) override;
	
	int32 GetRadius() { return _radius; }
	virtual ColliderType GetType() override { return ColliderType::Circle; }

private:
	int32		_radius;	// 반지름
};

