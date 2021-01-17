#include "EditorConfig.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleRenderExercise.h"
#include "ModuleWindow.h"
#include "ModuleEditorCamera.h"
#include "ModuleModel.h"
#include "ModuleInput.h"
#include "Mesh.h"
#include "ModuleScene.h"
#include "Material.h"

static void HelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

void EditorConfig::Draw(int w, int h)
{
	float fW = (float)w;
	float fH = (float)h;

	fW = fW / 5;
	fH = fH - (fH / 4);
	ImVec2 size(fW, fH);

	ImVec2 pos(0.0f, menuSize);
	ImGui::SetNextWindowPos(pos);

	if (!ImGui::Begin("Configuration", active, ImGuiWindowFlags_AlwaysUseWindowPadding))
	{
		ImGui::End();
		return;
	}
	ImGui::Text("Options");

	if (ImGui::CollapsingHeader("Application"))
	{
		char appName[50];
		strcpy_s(appName, 50, App->GetTitle());
		if (ImGui::InputText("Project Name", appName, IM_ARRAYSIZE(appName), ImGuiInputTextFlags_EnterReturnsTrue))
			App->SetTitle(appName);

		char organization[50];
		strcpy_s(organization, 50, App->GetOrganizatio());
		if (ImGui::InputText("Organization Name", organization, IM_ARRAYSIZE(organization), ImGuiInputTextFlags_EnterReturnsTrue))
			App->SetOrganizatio(organization);

		ImGui::Checkbox("vsync", &App->vsync);

		int fpsMax = App->GetFpsMax();
		if (ImGui::SliderInt("fps Max", &fpsMax, 1, 120))
			App->SetFpsMax(fpsMax);
		ImGui::SameLine(); HelpMarker("CTRL+click to input value.");

		unsigned int fps = App->GetFPS();
		ImGui::Text("FPS: "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i", fps);

		fpsLog.erase(fpsLog.begin());
		fpsLog.push_back(fps);
		fpsLog.size();

		msLog.erase(msLog.begin());
		msLog.push_back(1000.0f / fps);
		msLog.size();

		char title1[25];
		sprintf_s(title1, 25, "Framerate %i", fps);
		float arr1[100];
		std::copy(fpsLog.begin(), fpsLog.end(), arr1);
		ImGui::PlotHistogram("##framerate", arr1, IM_ARRAYSIZE(arr1), 0, title1, 0.0f, 100.0f, ImVec2(310, 100));

		char title2[25];
		sprintf_s(title2, 25, "Milliseconds %.3f", 1000.0f / fps);
		float arr2[100];
		std::copy(msLog.begin(), msLog.end(), arr2);
		ImGui::PlotHistogram("##milliseconds", arr2, IM_ARRAYSIZE(arr2), 0, title2, 0.0f, 100.0f, ImVec2(310, 100));
	}

	if (ImGui::CollapsingHeader("Render"))
	{
		bool aplhaTest = App->renderExercise->GetEnableAlphaTest();
		ImGui::Checkbox("AlphaTest", &aplhaTest);
		App->renderExercise->SetEnableAlphaTest(aplhaTest);

		bool cullFace = App->renderExercise->GetEnableCullFace();
		ImGui::Checkbox("CullFace", &cullFace);
		App->renderExercise->SetEnableCullFace(cullFace);

		bool depthTest = App->renderExercise->GetEnableDepthTest();
		ImGui::Checkbox("DepthTest", &depthTest);
		App->renderExercise->SetEnableDepthTest(depthTest);

		ImGui::Separator();

		float4 background = App->renderExercise->GetBackground();
		ImGui::ColorEdit3("Background", background.ptr());
		ImGui::SameLine(); HelpMarker(
			"Click on the color square to open a color picker.\n"
			"Click and hold to use drag and drop.\n"
			"Right-click on the color square to show options.\n"
			"CTRL+click on individual component to input value.\n");
		App->renderExercise->SetBackground(background);

		float3 lightDir = App->renderExercise->GetLightDir();
		ImGui::DragFloat3("Light Direction", lightDir.ptr(), 0.1f);
		App->renderExercise->SetLightDir(lightDir);

		float3 lightColor = App->renderExercise->GetLightColor();
		ImGui::ColorEdit3("Light Color", lightColor.ptr());
		App->renderExercise->SetLightColor(lightColor);

		float3 ambientColor = App->renderExercise->GetAmbientColor();
		ImGui::ColorEdit3("Ambient Color", ambientColor.ptr());
		App->renderExercise->SetAmbientColor(ambientColor);

		int shininess = App->renderExercise->GetShininess();
		ImGui::DragInt("Shininess", &shininess, 1);
		App->renderExercise->SetShininess(shininess);

		float3 diffuseColor = App->renderExercise->GetDiffuseColor();
		ImGui::ColorEdit3("Diffuse Color", diffuseColor.ptr());
		App->renderExercise->SetDiffuseColor(diffuseColor);

		float3 specularColor = App->renderExercise->GetSpecularColor();
		ImGui::ColorEdit3("Specular Color", specularColor.ptr());
		App->renderExercise->SetSpecularColor(specularColor);

		int hasDiffuseMap = App->renderExercise->GetHasDiffuseMap();
		ImGui::DragInt("Has Diffuse Map", &hasDiffuseMap, 1, 0, 1);
		App->renderExercise->SetHasDiffuseMap(hasDiffuseMap);

		int hasSpecularMap = App->renderExercise->GetHasSpecularMap();
		ImGui::DragInt("Has Specular Map", &hasSpecularMap, 1, 0, 1);
		App->renderExercise->SetHasSpecularMap(hasSpecularMap);

		int shininessApha = App->renderExercise->GetShininessApha();
		ImGui::DragInt("shininess Apha", &shininessApha, 1, 0, 1);
		App->renderExercise->SetShininessApha(shininessApha);
	}

	if (ImGui::CollapsingHeader("Window"))
	{
		static float brightness = App->window->GetBrightness();
		ImGui::SliderFloat("Brightness", &brightness, 0.0f, 1.0f, "%0.1f");
		App->window->SetBrightness(brightness);

		int width, height;
		SDL_GetWindowSize(App->window->window, &width, &height);
		ImGui::SliderInt("Width", &width, 0, 1920);
		ImGui::SliderInt("Height", &height, 0, 1080);
		App->window->SetWidthHeight(width, height);

		static bool fullScreen = App->window->GetWindowFull();
		static bool resizable = App->window->GetResizable();
		static bool borderless = App->window->GetBorderless();
		static bool fullDesktop = App->window->GetWindowFull();
		if (ImGui::Checkbox("FullScreen", &fullScreen)) {
			App->window->SetFullScreen(fullScreen);
			fullDesktop = false;
		}

		ImGui::SameLine();
		if (ImGui::Checkbox("Resizable", &resizable))
			App->window->SetResizable(resizable);


		if (ImGui::Checkbox("Borderless", &borderless))
			App->window->SetBorderless(borderless);

		ImGui::SameLine();
		if (ImGui::Checkbox("Full Desktop", &fullDesktop)) {
			App->window->SetFullDesktop(fullDesktop);
			fullScreen = false;
		}
	}

	if (ImGui::CollapsingHeader("Input"))
	{
		iPoint pos = App->input->GetMousePosition();
		ImGui::Text("Mouse Position "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "(x: %i, y: %i)", pos.x, pos.y);
	}

	if (ImGui::CollapsingHeader("Hardware"))
	{
		SDL_version linked;
		SDL_GetVersion(&linked);
		ImGui::Text("SDL Version: "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i.%i.%i", linked.major, linked.minor, linked.patch);

		ImGui::Separator();
		ImGui::Text("CPUs: "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i (cache: %ikb)", SDL_GetCPUCount(), SDL_GetCPUCacheLineSize());

		ImGui::Text("System RAM: "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i", SDL_GetSystemRAM());

		ImGui::Separator();
		ImGui::Text("GPU: "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", glGetString(GL_VENDOR));

		ImGui::Text("Brand: "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), " %s", glGetString(GL_RENDERER));
	}

	ImGui::SetWindowSize(size);
	ImGui::End();

}

