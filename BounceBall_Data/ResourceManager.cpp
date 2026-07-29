#include "pch.h"
#include "ResourceManager.h"
#include "Texture.h"
#include "Sound.h"

void ResourceManager::Init(HWND hwnd, fs::path directory)
{
	_resourcePath = directory;

	// 자동으로 Font 12,20,30 사이즈만큼 초기화
	LoadFont();

	// 빨간색 펜은 미리 만들어두고 사용만 하자
	_redPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
}

void ResourceManager::Cleanup()
{
	// 매니저가 생성했던 모든 텍스처를 해제해준다.
	for (auto iter : _textures)
	{
		// 실제 메모리 해제
		delete iter.second; // value : texture 
	}

	// 자료구조도 싹 비우고
	_textures.clear();
}

void ResourceManager::LoadTexture(wstring key, wstring texturePath, int32 transparent,
								  int32 row, int32 col, float dur, bool flip)
{
	if (GetTexture(key) != nullptr)
	{
		// 이미 추가된 텍스처라서 무시
		return;
	}

	fs::path fullPath = _resourcePath / "Image" / texturePath;

	// 텍스처 생성을 담당
	Texture* texture = new Texture(); 
	texture->Load(fullPath, transparent, row, col, dur, flip);

	// 생성된 텍스터를 리소스 매니저가 관리 N 개 관리
	_textures.insert(make_pair(key, texture));
}

Texture* ResourceManager::GetTexture(wstring key)
{
	auto find = _textures.find(key);
	if (find != _textures.end())
		return find->second;

	return nullptr;
}

Sound* ResourceManager::LoadSound(const wstring& key, const wstring& path)
{
	if (_sounds.find(key) != _sounds.end())
		return _sounds[key];

	fs::path fullPath = _resourcePath / "Sound" / path;

	Sound* sound = new Sound();
	sound->LoadWave(fullPath);
	_sounds[key] = sound;

	return sound;
}

void ResourceManager::LoadFont()
{
	//FR_PRIVATE: 시스템 전체가 아닌 현재 프로세스에서만 사용
	AddFontResourceEx((_resourcePath / L"Font/MaplestoryBold.ttf").c_str(), FR_PRIVATE, 0);

	auto createFont = [this](FontSize size) {

		// 이미 로드 완료
		if (_fonts.contains(size))
			return;

		HFONT hFont = CreateFont(
			(int32)size, 0, 0, 0, FW_NORMAL,
			FALSE, FALSE, FALSE,
			DEFAULT_CHARSET,
			OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,
			ANTIALIASED_QUALITY,
			DEFAULT_PITCH | FF_DONTCARE,
			L"메이플스토리"  // Family 이름!
		);
		_fonts[size] = hFont;
		};

	createFont(FontSize::FONT_12);
	createFont(FontSize::FONT_20);
	createFont(FontSize::FONT_30);
}
