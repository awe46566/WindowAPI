#include "pch.h"
#include "Widget.h"
#include "Texture.h"

void Widget::Init(wstring textureKey, Vector pos)
{
	_pos = pos;
	_image.Init(textureKey);
}

void Widget::Render(HDC hdc)
{
	_image.Render(hdc, _pos);
}

bool Widget::ContainsPoint(Vector point) const
{
	float halfX = _image.GetSizeX() * 0.5f;
	float halfY = _image.GetSizeY() * 0.5f;

	return point.x >= _pos.x - halfX && point.x <= _pos.x + halfX
		&& point.y >= _pos.y - halfY && point.y <= _pos.y + halfY;
}
