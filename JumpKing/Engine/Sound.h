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
	void Stop(bool reset = false);

private:
	virtual void LoadFile(const std::wstring& path) {}
	virtual void SaveFile(const std::wstring& path) {}

private:
	LPDIRECTSOUNDBUFFER _soundBuffer = nullptr;
	DSBUFFERDESC _bufferDesc = {};
};
