#include "SetupWindow.h"
#include "LoadingBar.h"
#include <filesystem>
#include "UI.h"
#include <NexusModsAPI.h>
#include "../WindowsFunctions.h"
#include "Net.h"
#include "Tabs/ModBrowserTab.h"
#include "../BackgroundTask.h"
#include "Game.h"

static SetupWindow* CurrentSetup = nullptr;
static thread_local std::string ApiKeyValue;

using namespace kui;

void SetupWindow::Init()
{
	if (CurrentSetup)
	{
		ShouldClose = true;
		return;
	}
	CurrentSetup = this;
	Element = new SetupWindowElement();

	CanClose = false;

	GenerateAPIKeySetupPage();
}

std::string SetupWindow::GetWindowTitle()
{
	return "Setup";
}

bool SetupWindow::ShouldOpen()
{
	return !std::filesystem::exists("app/saved/apikey.json");
}

void SetupWindow::AddText(std::string NewText, UIBox* Parent)
{
	if (Parent == nullptr)
	{
		Parent = Element->content;
	}

	Parent->AddChild((new UIText(11_px, 1, NewText, UI::Text))
		->SetWrapEnabled(true, 1.9f)
		->SetPadding(10_px, 5_px, 10_px, 10_px));
}

void SetupWindow::GenerateAccountInfo()
{
	InfoBox->DeleteChildren();

	std::string Name = NxmAPI::GetAPIKeyAccountName(ApiKeyValue);

	if (Name.empty())
	{
		AddText("The given API key doesn't exist.", InfoBox);
	}
	else
	{
		AddText("Account: " + Name, InfoBox);
	}
}

static bool ClickedNext = false;

void SetupWindow::GenerateAPIKeySetupPage()
{
	AddText("Please enter your NexusMods API key.");
	AddText("You can your the key by going to nexusmods.com, then to 'Site preferences' -> 'API KEYS' -> 'Personal API Key'");

	InputField = new UITextField(0, 0, UI::Text, [this]() {
		ApiKeyValue = InputField->GetText();

		GenerateAccountInfo();
		});
	Element->content->AddChild(InputField
		->SetHintText("API key here")
		->SetTextSize(12_px)
		->SetPadding(50_px, 50_px, 10_px, 10_px)
		->SetMinSize(Vec2f(1.75f, 0.15f))
		->SetMaxSize(Vec2f(1.75f, 1)));

	Element->nextButton->OnClicked = [this]()
		{
			if (ClickedNext)
			{
				return;
			}


			std::string Name = NxmAPI::GetAPIKeyAccountName(ApiKeyValue);

			if (Name.empty())
			{
				Windows::ErrorBox("The given API key does not exist.");
				return;
			}

			Element->content->DeleteChildren();
			ClickedNext = true;

			NxmAPI::SaveAPIKey(ApiKeyValue);
			Net::SetAPIKey(ApiKeyValue);
			AppTab::GetTabOfType<ModBrowserTab>()->ShouldReload = true;

			new BackgroundTask([this]() {
				auto LoadPopup = Popup::CreatePopup<LoadingBar>();
				LoadPopup->SetLoadingString("Looking for game...");

				std::string GameLocation = Game::GetGameLocation();

				LoadPopup->CanClose = true;
				LoadPopup->ShouldClose = true;

				if (std::filesystem::exists(GameLocation))
				{
					CanClose = true;
					ShouldClose = true;
					return;
				}

				Windows::ErrorBox("Could not automatically find the Cyberpunk 2077 game installation. Please enter the game's path manually.");

				while (true)
				{
					GameLocation = Windows::OpenFileDialog(true);

					if (std::filesystem::exists(GameLocation)
						&& std::filesystem::exists(GameLocation + "/bin/x64/Cyberpunk2077.exe"))
					{
						CanClose = true;
						ShouldClose = true;
						Game::SetGameLocation(GameLocation);
						return;
					}
					else if (GameLocation.empty())
					{
						Windows::ErrorBox("Please enter the game's path manually");
					}
					else
					{
						Windows::ErrorBox("Not a valid game install");
					}
				}
				});
		};

	InfoBox = new UIBox(false);
	Element->content->AddChild(InfoBox);
}

void SetupWindow::GenerateGameLocatePage()
{
	AddText("Please enter your NexusMods API key.");
}
