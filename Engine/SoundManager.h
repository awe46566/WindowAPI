#pragma once

#include <pch.h>
#include "Engine/Singleton.h"
#include "Sound.h"

class SoundManager : public Singleton<SoundManager>
{
	friend class Singleton<SoundManager>;
	~SoundManager();
public:
	void Init(HWND hwnd);
	void Play(const std::string& key, bool loop = false);

	LPDIRECTSOUND GetSoundDevice() { return _soundDevice; }

private:
	LPDIRECTSOUND _soundDevice = nullptr;
	std::unordered_map<std::string, unique_ptr<Sound>> _sounds;
};