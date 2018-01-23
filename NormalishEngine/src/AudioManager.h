#pragma once
#include <fmod.hpp>
#include <cstdint>


class AudioManager
{
public:
	AudioManager();
	
	FMOD::System* global_system;
	FMOD::Sound* global_stream;
	FMOD::Channel* global_channel;

	int32_t Init();

	int32_t PlaySound(const char* audio_file, bool should_loop);

	void Terminate();
};
