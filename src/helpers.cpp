#include "helpers.h"

void PrintError(const char* msg)
{
	std::cout << "[ERROR] " << msg << '\n';
}

std::string GetFileContent(const char* path)
{
	std::ifstream file(path, std::ios::binary);
	if (file)
	{
		file.seekg(0, std::ios::end);
		std::string content;
		content.resize(file.tellg());
		file.seekg(std::ios::beg);
		file.read(&content[0], content.size());
		file.close();
		return content;
	}
}
