#pragma once
#include <atomic>
#include <string>
#include <thread>
#include <vector>

struct BackgroundTask
{
	size_t Type = 0;
	std::atomic<float> Progress = 0;
	std::string Status;
	std::thread* Thread = nullptr;
	void(*Callback)(void* UserData);
	void* UserData = nullptr;
	// Will point to the task that is being executed right now.
	thread_local static BackgroundTask* ThisThreadPtr;

	BackgroundTask(void (*Function)(void* UserData), void(*Callback)(void* UserData) = nullptr, void* UserData = nullptr);
	~BackgroundTask();

	static void SetProgress(float Progress);
	static void SetStatus(std::string NewStatus);

	static bool IsRunningTask;

	static bool IsBackgroundTask();

	static void UpdateTasks();

	static bool IsFunctionRunningAsTask(void (*Function)());
	static std::vector<BackgroundTask*> AllTasks;

private:
	static void TaskRun(void (*Function)(void* Data), void* UserData, BackgroundTask* Task);
};