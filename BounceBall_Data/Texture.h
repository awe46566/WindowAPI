#pragma once

class Texture
{
public:
	void Load(wstring texturePath, int32 transparent, int32 row, int32 col, float dur, bool enableFlip);
	void Render(HDC hdc, Vector pos, Vector srcPos = Vector(0,0), bool flipX = false);
	void RenderScreen(HDC hdc, Vector screenPos, Vector srcPos = Vector(0,0));

	uint32 GetSizeX() const { return _sizeX; }
	uint32 GetSizeY() const { return _sizeY; }
	void SetSize(int32 x, int32 y) { _sizeX = x; _sizeY = y; }

	SIZE GetFrameSize() { return SIZE(_frameSizeX, _frameSizeY); }
	void GetFrameCount(int32& outX, int32& outY) { outX = _col; outY = _row; }
	float GetDur() { return _dur; }

	void SetApplyCenter(bool apply) { _applyCenter = apply; }

private:
	HDC			_bitmapHdc = 0;
	HBITMAP		_bitmap = 0;

	// WinAPI 기본 렌더링 함수는 좌우반전(Flip)을 지원하지 않으니
	// 반전된 이미지를 따로 저장해둔다.
	HDC			_flipBitmapHdc = 0;
	HBITMAP		_flipBitmap = 0;

	int32		_transparent = -1;
	uint32		_bitmapSizeX = 0;
	uint32		_bitmapSizeY = 0;

	bool		_applyCenter = true;	// 그림을 그릴때 가운데 좌표기준으로 보정해달라.


	// sprite로 쪼개진 텍스처의 경우, 행/열 개수를 저장한다.
	int32		_col = 0;
	int32		_row = 0;
	int32		_frameSizeX = 0;
	int32		_frameSizeY = 0;
	float		_dur = 0;

	// 사용자가 원하는 크기
	int32		_sizeX = 0;
	int32		_sizeY = 0;
};


