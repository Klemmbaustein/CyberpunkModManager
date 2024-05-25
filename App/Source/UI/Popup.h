#pragma once
#include <KlemmUI/Window.h>
#include <thread>
#include <mutex>
#include <atomic>
#include <KlemmUI/UI/UITextField.h>

class Popup
{
	static void PopupStart(Popup* p);

protected:

	Popup();

public:
	bool CanClose = true;
	KlemmUI::UIBackground* PopupBackground = nullptr;

	virtual std::string GetWindowTitle() = 0;

	KlemmUI::Window* PopupWindow = nullptr;

	std::thread* PopupThread = nullptr;
	static std::mutex PopupMutex;
	static std::vector<Popup*> Popups;

	virtual Vector2ui GetWindowResolution();

	std::atomic<bool> ShouldClose = false;

	virtual void Init() = 0;

	virtual bool HasBorder();
	virtual void Destroy();

	virtual ~Popup();

	template<typename T>
	static T* CreatePopup()
	{
		std::lock_guard PopupGuard = std::lock_guard(PopupMutex);
		T* New = new T();
		std::thread* NewThread = new std::thread(&PopupStart, static_cast<Popup*>(New));
		New->PopupThread = NewThread;
		return New;
	}

	virtual void Update();

	static void UpdatePopups();
};
