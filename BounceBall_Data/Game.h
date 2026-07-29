#pragma once

#include "Singleton.h"


// 전체 게임 로직을 담당하는 클래스
class Game : public Singleton<Game>
{
	// Singleton 객체를 '친구'로 선언해서 private 접근 가능하게 열어준다.
	friend Singleton<Game>;

public:
	void Init(HWND hwnd);
	void Cleanup();

	void Update();
	void Render();

	HWND GetHwnd() const { return _hwnd; }

private:
	// 아무나 생성못하게 생성자/소멸자를 숨기자
	Game() = default;
	~Game() = default;

private:
	HWND _hwnd;	// 윈도우 핸들
	RECT _rect;		// 윈도우 크기
	
	HDC _hdc;		// 메인 도화지 (출력용)
	HDC _hdcBack;	// 실시간으로 그려지는 버퍼
	HBITMAP _bmpBack;	// back hdc가 사용하는 텍스처
};

