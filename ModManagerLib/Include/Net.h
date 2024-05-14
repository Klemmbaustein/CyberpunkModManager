#pragma once
#include <string>
#include <atomic>

namespace Net
{
	std::string Get(std::string Url, std::atomic<float>* Progress = nullptr, float ProgressFraction = 1);
	void GetFile(std::string Url, std::string OutPath, std::atomic<float>* Progress = nullptr, float ProgressFraction = 1);

	void SetAPIKey(std::string NewKey);
	
	bool GetIsActive();
}