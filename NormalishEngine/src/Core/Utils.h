#pragma once
#include <cstdint>


#define SALT "Normalish"
#define SIZE_OF_SALT 10

struct ConfigData
{
	uint32_t resolution_x;
	uint32_t resolution_y;
	uint32_t fullscreen;
	uint32_t vsync;

	ConfigData() : resolution_x(640), resolution_y(480), fullscreen(0), vsync(0) {}
};

template<class InputIterator, class T>
bool bool_find(InputIterator first, InputIterator last, const T& val)
{
	while (first != last)
	{
		if (*first == val)
			return true;

		++first;
	}

	return false;
}

uint32_t string_hash(const char* data, uint32_t data_length, const char* salt, uint32_t salt_length)
{
	uint32_t hash = 0;
	unsigned char* hash_ptr = reinterpret_cast<unsigned char*>(&hash);

	for (uint32_t i = 0; i < salt_length; i++)
		hash_ptr[i % 4] = hash_ptr[i % 4] ^ salt[i];

	for (uint32_t j = 0; j < data_length; j++)
		hash_ptr[j % 4] = hash_ptr[j % 4] ^ data[j];

	return hash;
}

void LoadConfigs(ConfigData Config)
{
	std::ifstream file(".\\res\\config.txt");

	uint32_t resolution_x = string_hash("resolution_x", 13, SALT, SIZE_OF_SALT);
	uint32_t resolution_y = string_hash("resolution_y", 13, SALT, SIZE_OF_SALT);
	uint32_t fullscreen = string_hash("fullscreen", 11, SALT, SIZE_OF_SALT);
	uint32_t vsync = string_hash("vsync", 6, SALT, SIZE_OF_SALT);

	if (file.is_open())
	{
		std::string line;
		while (std::getline(file, line))
		{
			uint32_t space = line.find(' ');
			uint32_t name_hash = string_hash(line.substr(0, space).c_str(), space, SALT, SIZE_OF_SALT);
			std::string value = line.substr(space);

			if (name_hash == resolution_x)
				Config.resolution_x = std::stoi(value);
			if (name_hash == resolution_y)
				Config.resolution_y = std::stoi(value);
			if (name_hash == fullscreen)
				Config.fullscreen = std::stoi(value);
			if (name_hash == vsync)
				Config.vsync = std::stoi(value);
		}
	}
}
