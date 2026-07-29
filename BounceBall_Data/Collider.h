#pragma once
#include "Component.h"

// 모든 충돌체들은 Collider로 부터 파생된다.
class Collider : public Component
{
public:

	static bool CheckCircleToCircle(class ColliderCircle* circle, ColliderCircle* other, HitResult& result);
	static bool CheckCircleToAABB(class ColliderCircle* circle, class ColliderAABB* box, HitResult& result);

	class Actor* GetOwner() { return _owner; }

	// 무조건, Collider 확장한 클래스는 본인의 타입을 반환하도록 한다.
	virtual ColliderType GetType() = 0;
	
	// 실제로 충돌체크가 필요한 셀인지
	bool CheckCell() { return _checkCell; }
	void SetCheckCell(bool flag) { _checkCell = flag; }

protected:
	class Actor* _owner = nullptr;

	// 충돌 매니저에서, 실제로 충돌을 실행하는 '주체'
	bool		_checkCell = false;
};

