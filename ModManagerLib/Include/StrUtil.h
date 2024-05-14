#pragma once
#include <string>

namespace StrUtil
{
	std::string Replace(std::string String, std::string From, std::string To);
	std::string ShortenIfTooLong(std::string Target, size_t MaxLength);
}