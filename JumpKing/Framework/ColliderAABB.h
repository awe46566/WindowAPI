#pragma once

#include "pch.h"
#include "Framework/Component.h"

class Collider : public Component
{
public:
	void SetSize(const Vector2& size) { _size = size; }
	void SetOffset(const Vector2& offset) { _offset = offset; }

	Rect GetBounds(const Vector2& ownerPosition) const
	{
		return Rect
		{
			ownerPosition.x + _offset.x,
			ownerPosition.y + _offset.y,
			_size.x,
			_size.y
		};
	}
private:
	Vector2 _size;
	Vector2 _offset;
};