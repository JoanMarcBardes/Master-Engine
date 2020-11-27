#include "ModuleEditor.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleRenderExercise.h"
#include "ModuleWindow.h"
#include "ModuleEditorCamera.h"
#include "ModuleModel.h"
#include "ModuleInput.h"
#include "Mesh.h"
#include "SDL.h"
#include "GL/glew.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_sdl.h"
#include "ImGui/imgui_impl_opengl3.h"
#include "DebugLeaks.h"
#include <vector>
#include <string>


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

ModuleEditor::ModuleEditor()
{
	fpsLog.resize(fpsLogSize);
	msLog.resize(fpsLogSize);
}

// Destructor
ModuleEditor::~ModuleEditor()
{
	ClearLog();
}

// Called before render is available
bool ModuleEditor::Init()
{
	ImGui::CreateContext();

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking

	//void* context = App->renderer->GetContext();
	void* context = App->renderExercise->GetContext();
	ImGui_ImplSDL2_InitForOpenGL(App->window->window, context);
	ImGui_ImplOpenGL3_Init();
	return true;
}

update_status ModuleEditor::PreUpdate()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();

	return UPDATE_CONTINUE;
}

// Called every draw update
update_status ModuleEditor::Update()
{
	//ImGui::ShowDemoWindow();
    bool show = true;
	update_status status = MainMenuBar();

	if (showWindowConsole) WindowConsole(&showWindowConsole);
	if (showWindowConfiguration) WindowConfiguration(&showWindowConfiguration);
	if (showAbout) WindowAbout(&showAbout);

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	return status;
}

update_status ModuleEditor::PostUpdate()
{
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleEditor::CleanUp()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	return true;
}

void ModuleEditor::WindowConsole(bool* p_open)
{
	ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
	if (!ImGui::Begin("Console", p_open))
	{
		ImGui::End();
		return;
	}

	if (ImGui::SmallButton("Clear")) { ClearLog(); }

	ImGui::Separator();

	// Reserve enough left-over height for 1 separator + 1 input text
	const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y;
	ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar);

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
	for (int i = 0; i < Items.Size; i++)
	{
		const char* item = Items[i];

		ImVec4 color;
		bool has_color = false;
		if (strstr(item, "[error]")) { color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f); has_color = true; }
		else if (strncmp(item, "# ", 2) == 0) { color = ImVec4(1.0f, 0.8f, 0.6f, 1.0f); has_color = true; }
		if (has_color)
			ImGui::PushStyleColor(ImGuiCol_Text, color);
		ImGui::TextUnformatted(item);
		if (has_color)
			ImGui::PopStyleColor();
	}

	if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
		ImGui::SetScrollHereY(1.0f);

	ImGui::PopStyleVar();
	ImGui::EndChild();
	ImGui::End();
}

void ModuleEditor::ClearLog()
{
	for (int i = 0; i < Items.Size; i++)
		free(Items[i]);
	Items.clear();
}

void ModuleEditor::AddLog(const char* fmt, ...)// IM_FMTARGS(2)
{
	// FIXME-OPT
	char buf[1024];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
	buf[IM_ARRAYSIZE(buf) - 1] = 0;
	va_end(args);
	Items.push_back(Strdup(buf));
}

void ModuleEditor::WindowConfiguration(bool* p_open)
{
	if (!ImGui::Begin("Configuration", p_open, ImGuiWindowFlags_AlwaysUseWindowPadding))
	{
		ImGui::End();
		return;
	}
	ImGui::Text("Options");

	if (ImGui::CollapsingHeader("Application"))
	{
		static char appName[128] = "Joan Marc Engine";
		ImGui::InputText("App Name", appName, IM_ARRAYSIZE(appName));

		static char organization[128] = "UPC talent";
		ImGui::InputText("Organization", organization, IM_ARRAYSIZE(organization));

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
		App->window->SetWidthHeight(width,height);

		static bool fullScreen = App->window->GetWindowFull();
		static bool resizable = App->window->GetResizable();
		static bool borderless = App->window->GetBorderless();
		static bool fullDesktop = App->window->GetWindowFull();
		if (ImGui::Checkbox("FullScreen", &fullScreen)) {
			App->window->SetFullScreen(fullScreen);
			fullDesktop = false;
		}

		ImGui::SameLine(); 
		if(ImGui::Checkbox("Resizable", &resizable))
			App->window->SetResizable(resizable);


		if(ImGui::Checkbox("Borderless", &borderless))
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
		bool orbit = App->editorCamera->GetOrbit();
		ImGui::Checkbox("Orbit", &orbit);
		App->editorCamera->SetOrbit(orbit);

		static bool active = false;
		ImGui::Checkbox("Active", &active);

		float3 front = App->editorCamera->GetFront();
		ImGui::DragFloat3("Front", front.ptr());
		App->editorCamera->SetFront(front);

		float3 up = App->editorCamera->GetUp();
		ImGui::DragFloat3("Up", up.ptr());
		App->editorCamera->SetUp(up);

		float3 pos = App->editorCamera->GetPosition();
		ImGui::DragFloat3("Position", pos.ptr());
		App->editorCamera->SetPosition(pos);

		float3 direction = App->editorCamera->GetDiection();
		ImGui::DragFloat3("Direction", direction.ptr());
		App->editorCamera->SetDiection(direction);

		float yaw = App->editorCamera->GetYaw();
		ImGui::DragFloat("Yaw", &yaw, 0.01f);
		App->editorCamera->SetYaw(yaw);

		float pitch = App->editorCamera->GetPitch();
		ImGui::DragFloat("Pitch", &pitch, 0.01f);
		App->editorCamera->SetPitch(pitch);

		float movementSpeed = App->editorCamera->GetMovementSpeed();
		ImGui::DragFloat("Mov Speed", &movementSpeed, 0.01f);
		App->editorCamera->SetMovementSpeed(movementSpeed);

		float rotationSpeed = App->editorCamera->GetRotationSpeed();
		ImGui::DragFloat("Rot Speed", &rotationSpeed, 0.005f);
		App->editorCamera->SetRotationSpeed(rotationSpeed);

		float zoomSpeed = App->editorCamera->GetZoomSpeed();
		ImGui::DragFloat("Zoom Speed", &zoomSpeed, 0.005f);
		App->editorCamera->SetZoomSpeed(zoomSpeed);

		static bool frustum = false;
		ImGui::Checkbox("Frustum Culling", &frustum);

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

		static float col1[3] = { 1.0f, 0.0f, 0.2f };
		ImGui::ColorEdit3("Background", col1);
		ImGui::SameLine(); HelpMarker(
			"Click on the color square to open a color picker.\n"
			"Click and hold to use drag and drop.\n"
			"Right-click on the color square to show options.\n"
			"CTRL+click on individual component to input value.\n");

		ImGui::Text("Current: "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "TODO");

		static bool activeCamera = false;
		ImGui::Checkbox("Is Active Camera", &activeCamera);
	}

	if (ImGui::CollapsingHeader("Models info"))
	{
		std::vector<Mesh> meshes = App->model->GetMeshes();
		
		for( int i = 0; i<meshes.size(); ++i)
		{
			if (ImGui::TreeNode((void*)(intptr_t)i, "Mesh %i", i))
			{
				ImGui::Text("Name: %s", meshes[i].GetName());
				ImGui::Separator();

				ImGui::Text("Transformation");
				float4x4 model = meshes[i].GetModel();
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
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i", meshes[i].GetNumVertices());

				ImGui::Text("Num indices: "); ImGui::SameLine();
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i", meshes[i].GetNumIndices());
				ImGui::Separator();

				ImGui::Text("Texture");
				std::vector<unsigned int> texturesIds = meshes[0].GetTexturesIds();
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

	ImGui::End();	
}

//10. There must be a general menu with option to quit, visit the github page, and get info about the
//		engine(�About�).It should also be able to turn on / off editor windows
update_status ModuleEditor::MainMenuBar()
{
	update_status status = UPDATE_CONTINUE;
	ImGui::BeginMainMenuBar();
	if (ImGui::BeginMenu("Help"))
	{		
		if (ImGui::MenuItem("Github repository"))
		{
			RequestBrowser("http://www.google.com");
		}
		ImGui::MenuItem("About Dear ImGui", NULL, &showAbout);

		if (ImGui::BeginMenu("Windows"))
		{
			ImGui::MenuItem("bWindowConsole", "", &showWindowConsole);
			ImGui::MenuItem("WindowConfiguration", "", &showWindowConfiguration);
			ImGui::EndMenu();
		}
		if (ImGui::MenuItem("Quit", "Alt+F4"))
		{
			status = UPDATE_STOP;
		}
		ImGui::EndMenu();
	}
	ImGui::EndMainMenuBar();
	return status;
}

void ModuleEditor::WindowAbout(bool* p_open)
{
	if (!ImGui::Begin("About Dear ImGui", p_open, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::End();
		return;
	}
	ImGui::Text("Options");

	ImGui::End();
}

void ModuleEditor::RequestBrowser(const char* url)
{
	ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWDEFAULT);
}
