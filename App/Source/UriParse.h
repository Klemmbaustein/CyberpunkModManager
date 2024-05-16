#pragma once
#include <string>
#include <vector>
#include <map>

struct Uri
{
	std::string Scheme;
	std::string Authority;
	std::vector<std::string> Path;
	std::map<std::string, std::string> Query;
	std::string UriString;

	Uri(std::string UriString);
private:
	std::vector<std::string> ParsePath(std::string PathString, char Separator);
};