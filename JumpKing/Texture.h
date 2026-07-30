#pragma once
#include "Engine/Types.h"
#include "pch.h"

class Texture
{
public:
	void Load(const wstring& path);
	void Render(HDC hdc, const Vector2& position, const RECT& sourceRect);
	
	int32 GetWidth() const;
	int32 GetHeight() const;
	SIZE GetFrameSize() { return SIZE(_frameWidth, _frameHeight); }

private:
	unique_ptr<Gdiplus::Image> _image;

	uint32 _width = 0;
	uint32 _height = 0;

	int32 _row = 1;
	int32 _col = 1;
	int32 _frameWidth = 0;
	int32 _frameHeight = 0;
};