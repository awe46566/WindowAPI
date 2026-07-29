#pragma once

#include "Singleton.h"

class SoundManager : public Singleton<SoundManager>
{
	friend class Singleton<SoundManager>;
	~SoundManager();

public:
	void Init(HWND hwnd);

	void Play(const wstring& key, bool loop = false);

	LPDIRECTSOUND GetSoundDevice() { return _soundDevice; }

private:
	LPDIRECTSOUND _soundDevice = nullptr; // 사운드 카드
};

