#pragma once
#include <KlemmUI/UI/UIBackground.h>
#include "../UI.h"

class AppTab
{
public:

	static size_t SelectedTab;

	std::string Name;
	std::string IconFile;

	AppTab(std::string Name);
	virtual ~AppTab();

	virtual void Update();
	virtual void OnResized();

	static void ResizeAll();
	static void UpdateAll();
	
	static std::vector<AppTab*> AllTabs;
	KlemmUI::UIBackground* TabBackground = nullptr;

	template<typename T>
	static T* GetTabOfType()
	{
		for (AppTab* i : AllTabs)
		{
			if (dynamic_cast<T*>(i))
			{
				return static_cast<T*>(i);
			}
		}
		return nullptr;
	}

	template<typename T>
	static std::vector<T*> GetTabsOfType()
	{
		std::vector<T*> Tabs;
		for (AppTab* i : AllTabs)
		{
			if (dynamic_cast<T*>(i))
			{
				Tabs.push_back(static_cast<T*>(i));
			}
		}
		return Tabs;
	}

};