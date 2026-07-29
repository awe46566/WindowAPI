#include "pch.h"
#include "SoundManager.h"
#include "ResourceManager.h"
#include "Sound.h"

SoundManager::~SoundManager()
{
	if (_soundDevice)
		_soundDevice->Release();
}

void SoundManager::Init(HWND hwnd)
{
	// 사운드 디바이스 생성
	if (FAILED(::DirectSoundCreate(NULL, &_soundDevice, NULL)))
	{
		::MessageBox(NULL, L"사운드 디바이스 생성 실패", L"SYSTEM ERROR", MB_OK);
		return;
	}

	// 사운드 디바이스 협조 수준 설정
	if (FAILED(_soundDevice->SetCooperativeLevel(hwnd, DSSCL_PRIORITY)))
	{
		::MessageBox(NULL, L"사운드 디바이스 협조 수준 설정 실패", L"SYSTEM ERROR", MB_OK);
		return;
	}
}

// FMODE 
void SoundManager::Play(const wstring& key, bool loop /*= false*/)
{
	Sound* sound = ResourceManager::GetInstance().GetSound(key);
	if (sound == nullptr)
		return;

	sound->Play(loop);
}
