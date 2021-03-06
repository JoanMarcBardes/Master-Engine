#include "EditorMainMenu.h"
#include "Application.h"
#include "ModuleScene.h"

void EditorMainMenu::Draw(int w, int h)
{
	ImGui::BeginMainMenuBar();
	if (ImGui::BeginMenu("Help"))
	{
		if (ImGui::MenuItem("Github repository"))
		{
			RequestBrowser("https://github.com/JoanMarcBardes/Master-Engine");
		}
		ImGui::MenuItem("About", NULL, &activeAbout);

		if (ImGui::BeginMenu("Windows"))
		{
			ImGui::MenuItem("bWindowConsole", "", &activeConsole);
			ImGui::MenuItem("WindowConfiguration", "", &activeConfig);
			ImGui::MenuItem("WindowGameObjectHierarchy", "", &activeHierarchy);
			ImGui::MenuItem("WindowInspector", "", &activeInspector);
			ImGui::MenuItem("WindowPlayStopStep", "", &activePlay);
			ImGui::EndMenu();
		}
		if (ImGui::MenuItem("Quit", "Alt+F4"))
		{
			quit = true;
		}
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem("Save Scene"))
		{
			App->scene->Save();
		}
		if (ImGui::MenuItem("Load Scene"))
		{
			App->scene->Load();
		}
		ImGui::EndMenu();
	}

	menuSize = ImGui::GetWindowSize().y;
	ImGui::EndMainMenuBar();
}

void EditorMainMenu::RequestBrowser(const char* url)
{
	ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWDEFAULT);
}