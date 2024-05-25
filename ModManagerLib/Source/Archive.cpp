#include "Archive.h"
#include <filesystem>
#include <fstream>
#include "miniz/miniz.h"
#include <iostream>
#include "StrUtil.h"
#if _WIN32
#include <Windows.h>
#endif

void Archive::ExtractZip(std::string ArchivePath, std::string OutPath, std::atomic<float>* Progress, float ProgressFraction)
{
	mz_zip_archive archive;

	memset(&archive, 0, sizeof(mz_zip_archive));

	if (!mz_zip_reader_init_file(&archive, ArchivePath.c_str(), 0))
	{
		std::cout << "Failed to read .zip file: '" << ArchivePath
			<< "': " << mz_zip_get_error_string(mz_zip_get_last_error(&archive)) << std::endl;
		return;
	}

	const int FileCount = mz_zip_reader_get_num_files(&archive);

	if (FileCount <= 0)
		return;

	float StartProgress = 0;

	if (Progress)
	{
		StartProgress = *Progress;
	}

	mz_zip_archive_file_stat stats;

	for (int i = 0; i < FileCount; i++)
	{
		memset(&stats, 0, sizeof(stats));
		mz_zip_reader_file_stat(&archive, i, &stats);

		const bool IsDirectory = mz_zip_reader_is_file_a_directory(&archive, i);
		if (Progress)
		{
			*Progress = StartProgress + ((float)i / (float)FileCount) * ProgressFraction;
		}

		std::string TargetFileDir = OutPath + stats.m_filename;
		TargetFileDir = TargetFileDir.substr(0, TargetFileDir.find_last_of("\\/"));
		std::filesystem::create_directories(StrUtil::Replace(TargetFileDir, "\\", "/"));

		mz_zip_reader_extract_to_file(&archive, i, (OutPath + stats.m_filename).c_str(), 0);
	}

	mz_zip_reader_end(&archive);

}

void Archive::Extract(std::string ZipPath, std::string OutPath, std::atomic<float>* Progress, float ProgressFraction)
{
	std::cout << ZipPath << " -> " << OutPath << std::endl;
	std::string Extension = ZipPath.substr(ZipPath.find_last_of("/"));
	Extension = Extension.substr(Extension.find_first_of("."));
	if (Extension == ".zip")
	{
		ExtractZip(ZipPath, OutPath, Progress, ProgressFraction);
		return;
	}

#if _WIN32
	std::string Command = "app\\bin\\7z.exe x -y \"" + ZipPath + "\" \"-o" + OutPath + "\"";
	STARTUPINFOA Startup;
	PROCESS_INFORMATION pi;
	ZeroMemory(&Startup, sizeof(Startup));
	Startup.cb = sizeof(Startup);
	ZeroMemory(&pi, sizeof(pi));

	system(Command.c_str());
	/*
	CreateProcessA(NULL,
		(LPSTR)Command.c_str(),
		NULL,
		NULL,
		TRUE,
		CREATE_NO_WINDOW,
		NULL,
		NULL,
		&Startup,
		&pi);

	WaitForSingleObject(pi.hProcess, INFINITE);

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	*/
#else
	std::string Command = "app/bin/7zz x -y \"" + ZipPath + "\" \"-o" + OutPath + "\"";
	std::cout << Command << std::endl;
	system((Command).c_str());
#endif

	std::cout << "'" << Extension << "'" << std::endl;
	if (Extension == ".tar.gz")
	{
		std::cout << "Archive type is .tar.gz, extracting .tar file as well..." << std::endl;

		std::string FileName = ZipPath.substr(ZipPath.find_last_of("/\\") + 1);

		FileName = FileName.substr(0, FileName.find_first_of("."));

		Extract(OutPath + FileName + ".tar", OutPath, Progress, ProgressFraction);
	}
}