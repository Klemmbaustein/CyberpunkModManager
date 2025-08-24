#include "HomeTab.h"
#include "ModBrowserTab.h"
#include "../LoadingBar.h"
#include "../../BackgroundTask.h"
#include "../../WindowsFunctions.h"
#include <Game.h>
#include <Profile.h>
#include <NexusModsAPI.h>
#include <kui/App.h>
#include <filesystem>
#include <mutex>

using namespace kui;

HomeTab::HomeTab()
	: AppTab("Home")
{
	this->IconFile = "home.png";

	GamePart = new GameSection();

	GamePart->browse->button->OnClicked = [] {
		if (Game::HasGameLocation())
		{
			Windows::Open(Game::GetGameLocation());
		}
		else
		{
			Windows::ErrorBox("No game location set up!");
		}
		};

	GamePart->searchPc->button->OnClicked = [this] {
		new BackgroundTask([] {
			auto LoadBar = Popup::CreatePopup<LoadingBar>();
			auto Location = Game::SearchForGame();
			LoadBar->CanClose = true;
			LoadBar->ShouldClose = true;

			if (std::filesystem::exists(Location))
			{
				if (app::YesNoBox("Found location:\n" + Location + "\nIs this correct?", "Search for game"))
				{
					Game::SetGameLocation(Location);
					app::MessageBox(Location + ":\nLocation set successfully!", "Set game location", app::MessageType::Info);
				}
				return;
			}
			Windows::ErrorBox("Could not find a valid installation. Please set it manually.");
			}, [this] {
				this->UpdateGame();
				});
		};

	GamePart->setLocation->button->OnClicked = [this] {
		new BackgroundTask([] {
			auto Location = app::SelectFileDialog(true);

			if (Location.empty())
			{
				return;
			}

			if (!std::filesystem::exists(std::filesystem::path(Location) / "bin/x64/Cyberpunk2077.exe"))
			{
				Windows::ErrorBox(Location + ":\nNot a valid game installation!");
				return;
			}

			Game::SetGameLocation(Location);
			app::MessageBox(Location + ":\nLocation set successfully!", "Set game location", app::MessageType::Info);
			}, [this] {
				this->UpdateGame();
				});
		};

	GamePart->installedMods->button->OnClicked = [] {
		AppTab::SelectedTab = 1;
		};

	GamePart->onlineMods->button->OnClicked = [] {
		AppTab::SelectedTab = 2;
		};

	ProfilePart = new ProfileSection();

	this->TabBackground->AddChild(GamePart);
	this->TabBackground->AddChild(ProfilePart);
	this->TabBackground->SetHorizontalAlign(UIBox::Align::Centered);
	UpdateGame();
	UpdateAccount();
}

void HomeTab::UpdateGame()
{
	if (Game::HasGameLocation())
	{
		auto GamePath = Game::GetGameLocation();

		GamePart->SetHeaderText(std::filesystem::exists(GamePath + "/archive/pc/ep1") ? "Cyberpunk 2077 + Phantom Liberty" : "Cyberpunk 2077");
		GamePart->SetStatusText("Installed - " + GamePath);
		GamePart->SetModsText("Mods - " + std::to_string(Profile::Current.GetModsCount()) + " installed - Profile: " + Profile::Current.DisplayName);
	}
	else
	{
		GamePart->SetHeaderText("Cyberpunk 2077");
		GamePart->SetStatusText("Game location not set up. Provide a path or find it automatically to install mods.");
		GamePart->SetModsText("");
	}
}

void HomeTab::UpdateAccount()
{
	bool LoggedIn = !NxmAPI::GetAPIKey().empty();
	ProfilePart->buttons->DeleteChildren();

	if (LoggedIn)
	{
		static std::mutex AccountMutex;
		static std::string AccountName;

		new BackgroundTask(
			[&] {
				std::lock_guard g{ AccountMutex };
				AccountName = NxmAPI::GetAPIKeyAccountName(NxmAPI::GetAPIKey());
			},
			[&] {
				std::lock_guard g{ AccountMutex };

				if (AccountName.empty())
				{
					ProfilePart->SetStatusText("Unknown account or API error. Try again later or log out and in again.");
				}
				else
				{
					ProfilePart->SetStatusText("Logged in - " + AccountName);
				}
				UpdateProfileLoggedInButtons();
			});
		ProfilePart->SetStatusText("Loading...");

	}
	else
	{
		auto Buttons = new ProfileLoggedOutButtons();

		Buttons->login->button->OnClicked = [this] {
			MakeLoginForm();
			};
		ProfilePart->SetStatusText("Logged out");

		ProfilePart->buttons->AddChild(Buttons);
	}
}

void HomeTab::UpdateProfileLoggedInButtons()
{
	ProfilePart->buttons->DeleteChildren();

	auto Buttons = new ProfileLoggedInButtons();

	Buttons->logout->button->OnClicked = [this] {
		NxmAPI::SaveAPIKey("");
		AppTab::GetTabOfType<ModBrowserTab>()->ShouldReload = true;
		UpdateAccount();
		};

	ProfilePart->buttons->AddChild(Buttons);
}

void HomeTab::MakeLoginForm()
{
	ProfilePart->buttons->DeleteChildren();

	auto Form = new LoginForm();

	Form->loginButton->button->OnClicked = [this, Form] {
		auto Key = Form->apiKeyField->GetText();
		auto Name = NxmAPI::GetAPIKeyAccountName(Key);

		if (Name.empty())
		{
			Windows::ErrorBox("Invalid key.");
			return;
		}
		NxmAPI::SaveAPIKey(Key);
		AppTab::GetTabOfType<ModBrowserTab>()->ShouldReload = true;
		UpdateAccount();
		};

	Form->pageButton->button->OnClicked = [this] {
		Windows::Open("https://nexusmods.com");
		};

	Form->cancelButton->button->OnClicked = [this] {
		UpdateAccount();
		};

	ProfilePart->buttons->AddChild(Form);
}

void HomeTab::Update()
{
	if (ShouldReload)
	{
		this->UpdateGame();
		ShouldReload = false;
	}
}
