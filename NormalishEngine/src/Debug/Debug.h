#pragma once
#include <iostream>
#include <cstdint>
#include <fstream>
#include <tuple>
#include <vector>
#include "..\Core\Utils.h"
#include "..\Core\NMath.h"

#define SALT "Normalish"
#define SIZE_OF_SALT 10

std::vector<std::tuple<uint32_t, int32_t*>> hot_reload_int(5);
std::vector<std::tuple<uint32_t, float*>> hot_reload_float(5);
std::vector<std::tuple<uint32_t, bool*>> hot_reload_bool(5);
std::vector<std::tuple<uint32_t, vec3*>> hot_reload_vec3(5);


class Debug
{
public:
	Debug() {}

	static void AddHot(uint32_t name_hash, int32_t* var_ptr) { hot_reload_int.push_back(std::make_tuple(name_hash, var_ptr)); }
	static void AddHot(uint32_t name_hash, float* var_ptr) { hot_reload_float.push_back(std::make_tuple(name_hash, var_ptr)); }
	static void AddHot(uint32_t name_hash, bool* var_ptr) { hot_reload_bool.push_back(std::make_tuple(name_hash, var_ptr)); }
	static void AddHot(uint32_t name_hash, vec3* var_ptr) { hot_reload_vec3.push_back(std::make_tuple(name_hash, var_ptr)); }

	static void HotReload()
	{
		std::ifstream file(".\\res\\hotreload.txt");

		if (file.is_open())
		{
			std::string line;
			while (std::getline(file, line))
			{
				uint8_t space = line.find(' ');
				uint32_t name_hash = string_hash(line.substr(0, space).c_str(), space, SALT, SIZE_OF_SALT);
				std::string value = line.substr(space + 1);

				for (auto a : hot_reload_float)
				{
					if (std::get<0>(a) == name_hash)
						*std::get<1>(a) = std::stof(value);
				}
				for (auto b : hot_reload_int)
				{
					if (std::get<0>(b) == name_hash)
						*std::get<1>(b) = std::stoi(value);
				}
				for (auto c : hot_reload_bool)
				{
					if (std::get<0>(c) == name_hash)
						*std::get<1>(c) = static_cast<bool>(std::stoi(value));
				}
				for (auto d : hot_reload_vec3)
				{
					if (std::get<0>(d) == name_hash)
					{
						float vals[] = { 0.f, 0.f, 0.f };

						for (uint8_t i = 0; i < 3; i++)
						{
							uint8_t new_space = value.find(' ');
							std::string new_value = value.substr(0, new_space);
							vals[i] = std::stof(new_value);

							if (i < 2)
								value = value.substr(new_space + 1);
						}

						*std::get<1>(d) = vec3(vals[0], vals[1], vals[2]);
					}
				}

			}
			std::cout << "Hot reload success\n";
		}
		else
		{
			std::cout << "Error could not find file\n";
		}
	}
};
