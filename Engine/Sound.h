#pragma once

#include "pch.h"

class Sound
{
public:
	Sound();
	virtual ~Sound();

	Sound(const Sound&) = delete;
	Sound& operator=(const Sound&) = delete;

	bool loadWave(std::filesystem::path fullPath);
	void Play(bool loop = false);

private:
	LPDIRECTSOUNDBUFFER _soundBuffer = nullptr;
	DSBUFFERDESC _bufferDesc = {};
};
