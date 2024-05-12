#include "FileUtil.h"
#include <fstream>

std::string FileUtil::ReadFile(std::string FilePath)
{
	std::ifstream InStream = std::ifstream(FilePath);
	std::string FileString = std::string(std::istream_iterator<char>(InStream),
		std::istream_iterator<char>());
	InStream.close();
	return FileString;
}
