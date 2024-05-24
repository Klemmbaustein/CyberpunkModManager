#include "Net.h"
#include <fstream>
#include <vector>
#include <curl/curl.h>
#include "StrUtil.h"
#include <iostream>

#if _WIN32
#pragma comment(lib, "Wtsapi32.lib")
#pragma comment(lib, "wldap32.lib")
#pragma comment(lib, "crypt32.lib")
#pragma comment(lib, "Ws2_32.lib")
#endif

namespace Net
{
	std::string UserAgent = "KlemmCyberpunkModManager-Dev";
	bool IsActive = false;

	std::vector<std::string> Headers = { "user-agent: " + UserAgent };

	std::string ApiKeyHeader;
}

thread_local std::atomic<float>* ProgressVal = nullptr;
thread_local float ProgressFract = 1;
thread_local float StartProgress = 0;

static int ProgressFunction(void* ptr, curl_off_t TotalToDownload, curl_off_t NowDownloaded, curl_off_t TotalToUpload, curl_off_t NowUploaded)
{
	if (TotalToDownload == 0)
	{
		return 0;
	}
	if (ProgressVal)
	{
		*ProgressVal = StartProgress + ((float)NowDownloaded / (float)TotalToDownload) * ProgressFract;
	}
	return 0;
}

static size_t FileWrite(uint8_t* ptr, size_t size, size_t nmemb, std::ofstream* stream)
{
	stream->write((char*)ptr, size * nmemb);
	return size * nmemb;
}

static size_t StringWrite(uint8_t* ptr, size_t size, size_t nmemb, std::string* str)
{
	size_t NumBytes = size * nmemb;
	str->reserve(str->size() + NumBytes);
	for (size_t i = 0; i < NumBytes; i++)
	{
		str->push_back(ptr[i]);
	}
	return size * NumBytes;
}

static void DownloadInternal(std::string Url, std::vector<std::string> Headers, void* WriteFunction, void* WriteData)
{
	Url = StrUtil::Replace(Url, " ", "%20");
	CURL* CurlHandle = curl_easy_init();
	curl_slist* CurlHeaders = NULL;
	for (const std::string& Header : Headers)
	{
		CurlHeaders = curl_slist_append(CurlHeaders, Header.c_str());
	}
	curl_easy_setopt(CurlHandle, CURLOPT_HTTPHEADER, CurlHeaders);

	curl_easy_setopt(CurlHandle, CURLOPT_WRITEDATA, WriteData);
	curl_easy_setopt(CurlHandle, CURLOPT_FOLLOWLOCATION, true);

	curl_easy_setopt(CurlHandle, CURLOPT_URL, Url.c_str());
	curl_easy_setopt(CurlHandle, CURLOPT_WRITEFUNCTION, WriteFunction);
	curl_easy_setopt(CurlHandle, CURLOPT_NOPROGRESS, 0);
	curl_easy_setopt(CurlHandle, CURLOPT_XFERINFOFUNCTION, ProgressFunction);

	CURLcode res = curl_easy_perform(CurlHandle);

	curl_easy_cleanup(CurlHandle);
}
std::string Net::Get(std::string Url, bool IncludeApiKey, std::atomic<float>* Progress, float ProgressFraction)
{
	std::string OutString;

	if (Progress)
	{
		StartProgress = *Progress;
		ProgressFract = ProgressFraction;
	}
	ProgressVal = Progress;
	std::vector<std::string> HeadersToSend = Headers;

	if (IncludeApiKey)
	{
		HeadersToSend.push_back(ApiKeyHeader);
	}

	DownloadInternal(Url, HeadersToSend, (void*)&StringWrite, &OutString);
	ProgressVal = nullptr;

	return OutString;
}

void Net::GetFile(std::string Url, std::string OutPath, bool IncludeApiKey, std::atomic<float>* Progress, float ProgressFraction)
{
	std::ofstream OutStream = std::ofstream(OutPath, std::ios::binary);

	if (Progress)
	{
		StartProgress = *Progress;
		ProgressFract = ProgressFraction;
	}
	ProgressVal = Progress;
	std::vector<std::string> HeadersToSend = Headers;

	if (IncludeApiKey)
	{
		HeadersToSend.push_back(ApiKeyHeader);
	}
	
	DownloadInternal(Url, HeadersToSend, (void*)&FileWrite, &OutStream);
	OutStream.close();
}

void Net::SetAPIKey(std::string NewKey)
{
	ApiKeyHeader = ("apiKey:" + NewKey);
	IsActive = true;
}

bool Net::GetIsActive()
{
	return IsActive;
}
