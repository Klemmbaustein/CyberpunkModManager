#include "StrUtil.h"

std::string StrUtil::Replace(std::string String, std::string From, std::string To)
{
	size_t pos = 0;
	while ((pos = String.find(From, pos)) != std::string::npos)
	{
		String.replace(pos, From.length(), To);
		pos += To.length();
	}
	return String;
}

std::string StrUtil::ShortenIfTooLong(std::string Target, size_t MaxLength)
{
	if (Target.size() <= MaxLength)
	{
		return Target;
	}

	Target = Target.substr(0, MaxLength - 3);
	Target.append("...");
	return Target;
}
