#pragma once

#include "Singleton.h"

class Sound;

// 텍스처 로드&관리
// 사운드 로드&관리
// 폰트 로드&관리
class ResourceManager : public Singleton<ResourceManager>
{
	// Singleton 객체를 '친구'로 선언해서 private 접근 가능하게 열어준다.
	friend Singleton<ResourceManager>;

public:
	void Init(HWND hwnd, fs::path directory);
	void Cleanup();

	// 텍스처
	void LoadTexture(wstring key, wstring texturePath, int32 transparent, 
					 int32 row = 1, int32 col = 1, float dur = 0, bool flip = false);
	class Texture* GetTexture(wstring key);


	// 사운드
	Sound* GetSound(const wstring& key) { return _sounds[key]; }
	Sound* LoadSound(const wstring& key, const wstring& path);


	// 폰트
	void LoadFont();
	HFONT GetFont(FontSize size)
	{
		if (_fonts.find(size) != _fonts.end())
		{
			return _fonts[size];
		}
		return nullptr;
	}

	// 펜
	HPEN GetRedPen() { return _redPen; }

	fs::path GetResourcePath() { return _resourcePath; }

private:
	// 아무나 생성못하게 생성자/소멸자를 숨기자
	ResourceManager() = default;
	~ResourceManager() = default;

private:
	// 모든 texture 리소스 폴더내에 있을꺼라, 루트 디렉터리 정보를 미리 만들어두자.
	fs::path _resourcePath;

	// 키 : path
	// Texture*
	unordered_map<wstring, Texture*> _textures;

	// sound
	unordered_map<wstring, Sound*> _sounds;

	// font
	unordered_map<FontSize, HFONT> _fonts;

	// Pen
	HPEN _redPen;
};

