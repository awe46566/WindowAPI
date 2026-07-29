#include "pch.h"
#include "Collider.h"
#include "ColliderCircle.h"
#include "ColliderAABB.h"
#include "Actor.h"

bool Collider::CheckCircleToCircle(ColliderCircle* circle, ColliderCircle* other, HitResult& result)
{
	Vector size = circle->GetOwner()->GetPos() - other->GetOwner()->GetPos();
	float distance = size.Length();  // 두 액터의 거리 계산

	result.depth = circle->GetRadius() + other->GetRadius() - distance;

	// 반지름 두개 합친 길이보다, 거리가 짧다면, 겹친것이다.
	if (distance < circle->GetRadius() + other->GetRadius())
	{
		return true;
	}

	return false;
}

bool Collider::CheckCircleToAABB(ColliderCircle* circle, ColliderAABB* box, HitResult& result)
{
	// 원의 중심과 AABB 가장 가까운 AABB박스위의 최근접점을 찾는다.
	Vector center = circle->GetOwner()->GetPos();
	float radius = (float)circle->GetRadius();

	Vector boxMin = box->GetMin();
	Vector boxMax = box->GetMax();

	// AABB 에서 원의 중심과 가장 가까운 점 (중심을 AABB 범위로 클램프)
	Vector closest;
	closest.x = std::clamp(center.x, boxMin.x, boxMax.x);
	closest.y = std::clamp(center.y, boxMin.y, boxMax.y);

	// 원의 중심과 최근접점 거리를 계산
	Vector delta = center - closest;

	// 성능상 루트를 씌우는 sqrt() 함수를 호출하는것보다,
	// 제곱된 거리를 비교하는게 더 성능상 이점이 있어서, 굳이 진짜 거리를 계산하지않는다.
	float distSquared = delta.LengthSquared();

	// 거리가 < 원의 반지름 -> 충돌
	// 거리가 > 원의 반지름 -> 충돌 X
	if (distSquared >= radius * radius)
		return false;
	
	// 겹친 거리를 계산해서 반환한다.
	float distance = ::sqrt(distSquared);
	result.depth = radius - distance;

	if (distance < SMALL_NUMBER)
	{
		// 원의 중심이 사각형안에 포함된 경우, 
		// 중심과 근접점이 동일하니깐 normal 벡터 계산이 안된다.
		// 
		// 원의 중심이 AABB 내부까지 파고든 경우 (빠르게 이동했을 때 발생 가능)
		// 4개의 면 중에서 가장 얕게 겹친 면의 방향으로 밀어낸다.
		// -left = center.x - boxMin.x  →중심에서 왼쪽 벽까지 거리
		// - right = boxMax.x - center.x  →중심에서 오른쪽 벽까지 거리
		// - top = center.y - boxMin.y  →중심에서 위쪽 벽까지 거리
		// - bottom = boxMax.y - center.y  →중심에서 아래쪽 벽까지 거리

		//  이 4개 중 가장 작은 값(minOverlap)이 "탈출하기 가장 가까운 벽" 
		// -> 원을 그 방향으로 밀어내는 게 가장 자연스럽고(이동 거리가 최소)

		float left = center.x - boxMin.x;
		float right = boxMax.x - center.x;
		float top = center.y - boxMin.y;
		float bottom = boxMax.y - center.y;

		float minOverlap = std::min({ left, right, top, bottom });

		if (minOverlap == left)			result.normal = Vector(-1.f, 0.f);
		else if (minOverlap == right)	result.normal = Vector(1.f, 0.f);
		else if (minOverlap == top)		result.normal = Vector(0.f, -1.f);
		else							result.normal = Vector(0.f, 1.f);

		result.depth = minOverlap + radius;
	}
	else
	{
		// 거리를 계산하기위해 이미 방향벡터는 구해놨다.
		delta.Normalize();
		result.normal = delta;
	}

	return true;
}
