#include "pch.h"
#include "Framework/CollisionManager.h"

bool CollisionManager::CheckAABBToAABB(const Rect& movingBounds, const Rect& staticBounds, HitResult& result) const
{
	bool overlaps =
		movingBounds.Right() > staticBounds.Left() &&
		movingBounds.Left() < staticBounds.Right() &&
		movingBounds.Bottom() > staticBounds.Top() &&
		movingBounds.Top() < staticBounds.Bottom();
	
	if (!overlaps)
		return false;

	const float overlapFromLeft = movingBounds.Right() - staticBounds.Left();
	const float overlapFromRight = staticBounds.Right() - movingBounds.Left();
	const float overlapFromBottom = movingBounds.Bottom() - staticBounds.Top();
	const float overlapFromTop = staticBounds.Bottom() - movingBounds.Top();

	const float minOverlap = min(
		{
			overlapFromLeft,
			overlapFromRight,
			overlapFromBottom,
			overlapFromTop
		}
	);

	result.depth = minOverlap;

	if (minOverlap == overlapFromLeft)
	{
		result.normal = Vector2{ -1.0f, 0.0f };
	}
	else if (minOverlap == overlapFromRight)
	{
		result.normal = Vector2{ 1.0f, 0.0f };
	}
	else if (minOverlap == overlapFromBottom)
	{
		result.normal = Vector2{ 0.0f, 1.0f };
	}
	else
	{
		result.normal = Vector2{ 0.0f, -1.0f };
	}

	return true;
}
