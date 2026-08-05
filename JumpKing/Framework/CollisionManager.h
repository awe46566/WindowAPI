#pragma once
#include "pch.h"
#include "Engine/Types.h"
#include "Engine/Singleton.h"

class CollisionManager : public Singleton<CollisionManager>
{
public:
	bool CheckAABBToAABB(const Rect& movingBounds, const Rect& staticBounds, HitResult& result) const;
	
};