#include "pch.h"
#include "SoundManager.h"
#include "Sound.h"
#include "ResourceCatalog.h"

SoundManager::~SoundManager()
{
	if (_soundDevice)
		_soundDevice->Release();
}

void SoundManager::Init(HWND hwnd)
{
	if (FAILED(::DirectSoundCreate(NULL, &_soundDevice, NULL)))
	{
		::MessageBox(NULL, L"사운드 디바이스 생성 실패", L"SYSTEM ERROR", MB_OK);
		return;
	}

	if (FAILED(_soundDevice->SetCooperativeLevel(hwnd, DSSCL_PRIORITY)))
	{
		::MessageBox(NULL, L"사운드 디바이스 협조 레벨 설정 실패", L"SYSTEM ERROR", MB_OK);
		return;
	}
}

void SoundManager::Play(const std::string& key, bool loop)
{
	auto found = _sounds.find(key);
	
	if (found != _sounds.end())
	{
		found->second->Play(loop);
		return;
	}
	
	auto resource = ResourceCatalog::GetInstance().FindSound(key);
	if (!resource)
		return;

	auto sound = std::make_unique<Sound>();
	
	if(!sound->loadWave(resource->path))
		return;
	
	sound->Play(loop);
	_sounds.emplace(key, std::move(sound));

}
