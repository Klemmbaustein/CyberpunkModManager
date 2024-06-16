#include "BackgroundTask.h"
#include <cassert>
#include "Error.h"
#include <iostream>

thread_local BackgroundTask* BackgroundTask::ThisThreadPtr = nullptr;
bool BackgroundTask::IsRunningTask;
std::vector<BackgroundTask*> BackgroundTask::AllTasks;

BackgroundTask::BackgroundTask(std::function<void()> Function, std::function<void()> Callback)
{
	AllTasks.push_back(this);
	this->Callback = Callback;

	Thread = new std::thread(TaskRun, Function, this);
}

BackgroundTask::~BackgroundTask()
{
	Thread->join();
	delete Thread;
}

void BackgroundTask::SetProgress(float Progress)
{
	assert(ThisThreadPtr);
	ThisThreadPtr->Progress = Progress;
}

void BackgroundTask::SetStatus(std::string NewStatus)
{
	assert(ThisThreadPtr);
	ThisThreadPtr->Status = NewStatus;
}

void BackgroundTask::TaskRun(std::function<void()> Function, BackgroundTask* ThisTask)
{
	Error::RegisterErrorHandler();
	ThisThreadPtr = ThisTask;
	try
	{
		Function();
	}
	catch (std::exception& e)
	{
		std::cout << "Background task failed: " << e.what() << std::endl;
	}
	ThisTask->Progress = 1;
}


bool BackgroundTask::IsBackgroundTask()
{
	return ThisThreadPtr;
}

void BackgroundTask::UpdateTasks()
{
	IsRunningTask = false;
	for (size_t i = 0; i < AllTasks.size(); i++)
	{
		if (AllTasks[i]->Progress >= 1)
		{
			auto Callback = AllTasks[i]->Callback;
			delete AllTasks[i];
			AllTasks.erase(AllTasks.begin() + i);
			if (Callback)
			{
				Callback();
			}
			break;
		}
		else if (!IsRunningTask)
		{
			IsRunningTask = true;
		}
	}
}