#include "BackgroundTask.h"
#include <cassert>
#include "Error.h"
#include <iostream>

thread_local BackgroundTask* BackgroundTask::ThisThreadPtr = nullptr;
bool BackgroundTask::IsRunningTask;
std::vector<BackgroundTask*> BackgroundTask::AllTasks;

BackgroundTask::BackgroundTask(void(*Function)(void* UserData), void(*Callback)(void* UserData), void* UserData)
{
	Type = (size_t)Function;
	AllTasks.push_back(this);
	this->Callback = Callback;
	this->UserData = UserData;

	Thread = new std::thread(TaskRun, Function, UserData, this);
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

void BackgroundTask::TaskRun(void (*Function)(void* Data), void* UserData, BackgroundTask* ThisTask)
{
	Error::RegisterErrorHandler();
	ThisThreadPtr = ThisTask;
	try
	{
		Function(UserData);
	}
	catch (std::exception& e)
	{
		std::cout << "Background task failed: " << e.what() << std::endl;
	}
	ThisTask->Progress = 1;
}

bool BackgroundTask::IsFunctionRunningAsTask(void(*Function)())
{
	for (auto& i : AllTasks)
	{
		if (i->Type == (size_t)Function)
		{
			return true;
		}
	}
	return false;
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
			void* Data = AllTasks[i]->UserData;
			delete AllTasks[i];
			AllTasks.erase(AllTasks.begin() + i);
			if (Callback)
			{
				Callback(Data);
			}
			break;
		}
		else if (!IsRunningTask)
		{
			IsRunningTask = true;
		}
	}
}