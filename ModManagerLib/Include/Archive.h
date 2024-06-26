#pragma once
#include <string>
#include <atomic>

namespace Archive
{
	void ExtractZip(std::string Archive, std::string OutPath, std::atomic<float>* Progress, float ProgressFraction);

	void Extract(std::string Archive, std::string OutPath, std::atomic<float>* Progress, float ProgressFraction);
}