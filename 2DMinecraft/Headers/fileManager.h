// Forrest Lowe 2021

#pragma once
#ifndef FILE_SYSTEM
#define FILE_SYSTEM

#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>

namespace FILESYSTEM
{
	static const std::string GetInputFromUser()
	{
		std::string input;
		std::cin >> input;

		return input;
	}

	static const int GetFileSize(const char* fileName)
	{
		std::ifstream fileStream(fileName, std::ifstream::ate | std::ifstream::binary);

		return static_cast<int>(fileStream.tellg());
	}

	static const std::string GetFileData(const char* fileName)
	{
		std::ifstream readFileStream(fileName);

		if (!readFileStream)
		{
			return "";
		}

		std::string data;
		std::string n;

		while (readFileStream >> n)
		{
			data += n;
		}

		readFileStream.close();

		return data;
	}

	static const std::string WriteFile(const char* fileName, std::string data, std::fstream::_Openmode openMode = std::fstream::app)
	{
		std::ofstream writeFileStream(fileName, openMode);
		double size = 0.0;

		if (writeFileStream.is_open())
		{
			writeFileStream << data.c_str();
			size = GetFileSize(fileName) * 0.001;

			writeFileStream.close();
		}
		else
		{
			return "WARNING! Could not create file! FileStream failure! May need more access.";
		}

		return "Success! File created with a size of: " + std::to_string(size) + " kb";
	}
}

#endif // !FILE_SYSTEM