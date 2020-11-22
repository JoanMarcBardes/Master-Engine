#include "ModuleEditor.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleRenderExercise.h"
#include "ModuleWindow.h"
#include "ModuleEditorCamera.h"
#include "ModuleModel.h"
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
	//WindowHello();
	//WindowConsole();
	MainMenuBar();
	WindowConfiguration();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	return UPDATE_CONTINUE;
}

update_status ModuleEditor::PostUpdate()
{
	//SDL_GL_MakeCurrent(App->window->window, App->renderer->GetContext());
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

////////////////
// Our state
bool show_demo_window = true;
bool show_another_window = false;
ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

void ModuleEditor::WindowHello()
{
	static float f = 0.0f;
	static int counter = 0;

	ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

	ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
	ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
	ImGui::Checkbox("Another Window", &show_another_window);

	ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
	ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

	if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
		counter++;
	ImGui::SameLine();
	ImGui::Text("counter = %d", counter);

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();
}

void ModuleEditor::WindowConsole()
{
	ImGui::Begin("Console");

	ImGui::Separator();
	if (ImGui::Button("ClearLog"))
		ClearLog();

	for (int i = 0; i < Items.Size; i++)
		ImGui::TextUnformatted(Items[i]);	
	
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

void ModuleEditor::WindowConfiguration()
{
	ImGui::Begin("Configuration");
	ImGui::Text("Options");

	if (ImGui::CollapsingHeader("Help"))
	{
		if (ImGui::Button("Button"))
			RequestBrowser("http://www.google.com");
		ImGui::Separator();
	}

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

		char title[25];
		sprintf_s(title, 25, "Framerate %i", fps);
		float arr[100];
		std::copy(fpsLog.begin(), fpsLog.end(), arr);
		ImGui::PlotHistogram("##framerate", arr, IM_ARRAYSIZE(arr), 0, title, 0.0f, 100.0f, ImVec2(310, 100));
		/*sprintf_s(title, 25, "Milliseconds %.1f", ms_log[ms_log.size() - 1]);
		ImGui::PlotHistogram("##framerate", &ms_log[0], ms_log.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));*/
	}

	if (ImGui::CollapsingHeader("Window"))
	{ 
		static bool active = false;
		ImGui::Checkbox("Active", &active);

		static float brightness = App->window->GetBrightness();
		ImGui::SliderFloat("Brightness", &brightness, 0.0f, 1.0f, "%0.1f");
		App->window->SetBrightness(brightness);

		//glutGet(GLUT_WINDOW_WIDTH)
		//glutGet(GLUT_WINDOW_HEIGHT)
		int width, height;
		SDL_GetWindowSize(App->window->window, &width, &height);

		//static int width = App->window->GetWidth(); 
		ImGui::SliderInt("Width", &width, 0, 1920);
		//static int height = App->window->GetHeight();
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

	ImGui::End();	
}

void ModuleEditor::MainMenuBar()
{
	ImGui::BeginMainMenuBar();
	if (ImGui::BeginMenu("Help"))
	{
		if (ImGui::MenuItem("Documentation"))
		{
			RequestBrowser("http://www.google.com");
		}
	}
	ImGui::EndMainMenuBar();
}

void ModuleEditor::RequestBrowser(const char* url)
{
	ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWDEFAULT);
}
