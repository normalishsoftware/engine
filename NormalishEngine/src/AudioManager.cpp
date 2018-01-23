#include "AudioManager.h"
#include <iostream>


AudioManager::AudioManager()
{
}

int32_t AudioManager::Init()
{
	FMOD::System_Create(&global_system);
	FMOD_RESULT result = global_system->init(100, FMOD_INIT_NORMAL, nullptr);
	if (result != FMOD_OK)
	{
		std::cout << "Failed to initialize FMOD\n";
		return -1;
	}

	return FMOD_OK;
}

int32_t AudioManager::PlaySound(const char* audio_file, bool should_loop)
{
	(!should_loop) ? global_system->createStream(audio_file, FMOD_DEFAULT | FMOD_LOOP_OFF, nullptr, &global_stream) : global_system->createStream(audio_file, FMOD_DEFAULT | FMOD_LOOP_NORMAL, nullptr, &global_stream);

	global_system->playSound(global_stream, nullptr, false, &global_channel);

	return FMOD_OK;
}

void AudioManager::Terminate()
{
	global_stream->release();
	global_system->release();
}
