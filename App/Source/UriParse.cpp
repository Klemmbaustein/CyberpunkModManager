#include "UriParse.h"
#include "WindowsFunctions.h"
#include <iostream>

Uri::Uri(std::string UriString)
{
	if (UriString.size() >= 2 && UriString[0] == '\'' && UriString[UriString.size() - 1] == '\'')
	{
		UriString = UriString.substr(1, UriString.size() - 2);
	}

	this->UriString = UriString;
	size_t Colon = UriString.find_first_of(":");
	if (Colon == std::string::npos || Colon == 0)
	{
		// URI does not have a scheme (Normally something like https://)
		Windows::ErrorBox("Could not parse URI:\n" + UriString);
		return;
	}

	Scheme = UriString.substr(0, Colon);

	UriString = UriString.substr(Colon + 1);

	if (UriString.substr(0, 2) != "//")
	{
		// URI does not have 2 slashes after the scheme (Example: https://)
		Windows::ErrorBox("Could not parse URI:\n" + UriString);
		return;
	}
	UriString = UriString.substr(2);

	size_t FirstSlash = UriString.find_first_of("/");

	Authority = UriString.substr(0, FirstSlash);

	if (FirstSlash == std::string::npos)
	{
		// URI is just the authority (like https://abc.xyz)
		return;
	}
	// URI contains a path (like https://abc.xyz/my/very/cool/path)

	std::string PathString = UriString.substr(FirstSlash);

	size_t QueryBegin = PathString.find_first_of("?");

	if (QueryBegin != std::string::npos)
	{
		// URI contains a query (like https://abc.xyz/something?query=hello&something=else
		std::string QueryString = PathString.substr(QueryBegin + 1);

		PathString = PathString.substr(0, QueryBegin);

		auto QueryVec = ParsePath(QueryString, '&');

		for (auto& i : QueryVec)
		{
			size_t EqualsPos = i.find_first_of("=");
			if (EqualsPos == std::string::npos || EqualsPos == 0)
			{
				continue;
			}
			Query.insert(std::pair(i.substr(0, EqualsPos), i.substr(EqualsPos + 1)));
		}
	}

	Path = ParsePath(PathString, '/');
}

std::vector<std::string> Uri::ParsePath(std::string PathString, char Separator)
{
	std::vector<std::string> OutPath;
	std::string CurrentEntry;

	for (char c : PathString)
	{
		if (c == Separator)
		{
			if (!CurrentEntry.empty())
			{
				OutPath.push_back(CurrentEntry);
			}
			CurrentEntry.clear();
			continue;
		}
		CurrentEntry.push_back(c);
	}
	if (!CurrentEntry.empty())
	{
		OutPath.push_back(CurrentEntry);
	}

	return OutPath;
}
