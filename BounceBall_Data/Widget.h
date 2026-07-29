#pragma once
#include "ImageRenderer.h"

class Widget
{
public:
	virtual void Init(wstring textureKey, Vector pos);
	virtual void Update(float deltaTime) {}
	virtual void Render(HDC hdc);

	// 텍스처 중심좌표(_pos) 기준 사각형 안에 point가 있는지 검사
	bool ContainsPoint(Vector point) const;

public:
	Vector _pos;
	ImageRenderer _image;
};

