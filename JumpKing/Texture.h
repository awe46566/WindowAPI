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

private:
	unique_ptr<Gdiplus::Image> _image;
};