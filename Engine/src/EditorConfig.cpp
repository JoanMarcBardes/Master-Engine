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

	if (!ImGui::Begin("Configuration", active, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoMove))
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

		int fpsMax = App->GetFpsMax();
		if (ImGui::SliderInt("fps Max", &fpsMax, 1, 60))
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

		float4 lightColor = App->renderExercise->GetLightColor();
		ImGui::ColorEdit3("Light Color", lightColor.ptr());
		App->renderExercise->SetLightColor(lightColor);

		float4 ambientColor = App->renderExercise->GetAmbientColor();
		ImGui::ColorEdit3("Ambient Color", ambientColor.ptr());
		App->renderExercise->SetAmbientColor(ambientColor);

		float Ks = App->renderExercise->GetKs();
		ImGui::DragFloat("Ks", &Ks, 0.01f);
		App->renderExercise->SetKs(Ks);

		float Kd = App->renderExercise->GetKd();
		ImGui::DragFloat("Kd", &Kd, 0.01f);
		App->renderExercise->SetKd(Kd);

		int shininess = App->renderExercise->GetShininess();
		ImGui::DragInt("Shininess", &shininess, 1);
		App->renderExercise->SetShininess(shininess);
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

	if (ImGui::CollapsingHeader("Camera"))
	{
		float3 front = App->editorCamera->GetFront();
		ImGui::DragFloat3("Front", front.ptr());
		App->editorCamera->SetFront(front);

		float3 up = App->editorCamera->GetUp();
		ImGui::DragFloat3("Up", up.ptr());
		App->editorCamera->SetUp(up);

		float3 pos = App->editorCamera->GetPosition();
		ImGui::DragFloat3("Position", pos.ptr());
		App->editorCamera->SetPosition(pos);

		ImGui::Separator();

		float nearPlane = App->editorCamera->GetNearPlane();
		ImGui::DragFloat("Near Plane", &nearPlane, 0.1f);
		App->editorCamera->SetNearPlane(nearPlane);

		float farPlane = App->editorCamera->GetFarPlane();
		ImGui::DragFloat("Far Plane", &farPlane, 1.0f);
		App->editorCamera->SetFarPlane(farPlane);

		float fov = App->editorCamera->GetFOV();
		ImGui::DragFloat("Field of View", &fov, 1.0f);
		App->editorCamera->SetFOV(fov);

		float ascpectRatio = App->editorCamera->GetAspectRatio();
		ImGui::DragFloat("Aspect Ratio", &ascpectRatio, 0.01f);
		App->editorCamera->SetAspectRatio(ascpectRatio);

		float speed = App->editorCamera->GetSpeed();
		ImGui::DragFloat("Movement Speed", &speed, 0.01f);
		App->editorCamera->SetSpeed(speed);
	}

	if (ImGui::CollapsingHeader("Models info"))
	{
		std::vector<Mesh*> meshes = App->model->GetMeshes();
		int i = 0;
		for (std::vector<Mesh*>::iterator it = meshes.begin(); it != meshes.end(); ++it)
		{
			if (ImGui::TreeNode((void*)(intptr_t)i, "Mesh %i", i))
			{
				ImGui::Text("Name: %s", (*it)->GetName());
				ImGui::Separator();

				ImGui::Text("Transformation");
				float4x4 model = (*it)->GetModel();
				float3 translate;
				float4x4 rotate;
				float3 sacale;
				model.Decompose(translate, rotate, sacale);

				ImGui::InputFloat3("Position", translate.ptr());
				ImGui::InputFloat3("Rotate", rotate.ToEulerXYZ().ptr());
				ImGui::InputFloat3("Scale", sacale.ptr());
				ImGui::Separator();

				ImGui::Text("Geometry");
				ImGui::Text("Num vertices: "); ImGui::SameLine();
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i", (*it)->GetNumVertices());

				ImGui::Text("Num indices: "); ImGui::SameLine();
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i", (*it)->GetNumIndices());
				ImGui::Separator();

				ImGui::Text("Texture");
				std::vector<unsigned int> texturesIds = App->model->GetTexturesIds();
				float my_tex_w = 150;
				float my_tex_h = 150;
				for (int j = 0; j < texturesIds.size(); ++j)
				{
					if (j != 0) ImGui::SameLine();
					ImVec2 pos = ImGui::GetCursorScreenPos();
					ImVec2 uv_min = ImVec2(0.0f, 0.0f);                 // Top-left
					ImVec2 uv_max = ImVec2(1.0f, 1.0f);                 // Lower-right
					ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
					ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f); // 50% opaque white
					ImGui::Image((ImTextureID)texturesIds[j], ImVec2(my_tex_w, my_tex_h), uv_min, uv_max, tint_col, border_col);
				}

				ImGui::TreePop();
			}
			++i;
		}
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

