#include "Archive.h"
#include <filesystem>
#include "miniz/miniz.h"

void Archive::Extract(std::string Archive, std::string OutPath, std::atomic<float>* Progress, float ProgressFraction)
{
	mz_zip_archive archive;

	memset(&archive, 0, sizeof(mz_zip_archive));

	mz_zip_reader_init_file(&archive, Archive.c_str(), 0);

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
		std::filesystem::create_directories(TargetFileDir);

		mz_zip_reader_extract_to_file(&archive, i, (OutPath + stats.m_filename).c_str(), 0);
	}

	mz_zip_reader_end(&archive);

}
