#include "ModuleEditor.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleRenderExercise.h"
#include "ModuleWindow.h"
#include "ModuleEditorCamera.h"
#include "SDL.h"
#include "GL/glew.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_sdl.h"
#include "ImGui/imgui_impl_opengl3.h"
#include "DebugLeaks.h"


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
	//WindowConfiguration();

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
		ImGui::Text("ABOUT THIS DEMO:");
		ImGui::BulletText("Sections below are demonstrating many aspects of the library.");
		ImGui::BulletText("The \"Examples\" menu above leads to more demo contents.");
		ImGui::BulletText("The \"Tools\" menu above gives access to: About Box, Style Editor,\n"
			"and Metrics (general purpose Dear ImGui debugging tool).");
		ImGui::Separator();

		ImGui::Text("PROGRAMMER GUIDE:");
		ImGui::BulletText("See the ShowDemoWindow() code in imgui_demo.cpp. <- you are here!");
		ImGui::BulletText("See comments in imgui.cpp.");
		ImGui::BulletText("See example applications in the examples/ folder.");
		ImGui::BulletText("Read the FAQ at http://www.dearimgui.org/faq/");
		ImGui::BulletText("Set 'io.ConfigFlags |= NavEnableKeyboard' for keyboard controls.");
		ImGui::BulletText("Set 'io.ConfigFlags |= NavEnableGamepad' for gamepad controls.");
		ImGui::Separator();

		ImGui::Text("USER GUIDE:");
		ImGui::ShowUserGuide();
	}

	if (ImGui::CollapsingHeader("Application"))
	{
		static char appName[128] = "Joan Marc Engine";
		ImGui::InputText("App Name", appName, IM_ARRAYSIZE(appName));

		static char organization[128] = "UPC talent";
		ImGui::InputText("Organization", organization, IM_ARRAYSIZE(organization));

		static int fps = 0;
		ImGui::SliderInt("Max FPS", &fps, 0, 120);
		ImGui::SameLine(); HelpMarker("CTRL+click to input value.");

		ImGui::Text("Limit Framerate: "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i", fps);

		/*char title[25];
		sprintf_s(title, 25, "Framerate %.1f", fps_log[fps_log.size() - 1]);
		ImGui::PlotHistogram("##framerate", &fps_log[0], fps_log.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));
		sprintf_s(title, 25, "Milliseconds %.1f", ms_log[ms_log.size() - 1]);
		ImGui::PlotHistogram("##framerate", &ms_log[0], ms_log.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));*/
	}

	if (ImGui::CollapsingHeader("Window"))
	{ 
		static bool active = false;
		ImGui::Checkbox("Active", &active);

		static float brightness = App->window->GetBrightness();
		ImGui::SliderFloat("Brightness", &brightness, 0.0f, 1.0f, "%0.1f");
		App->window->SetBrightness(brightness);

		static int width = App->window->GetWidth();
		ImGui::SliderInt("Width", &width, 0, 1920);
		static int height = App->window->GetHeight();
		ImGui::SliderInt("Height", &height, 0, 1080);
		App->window->SetWidthHeight(width,height);


		int refressRate = 59;
		ImGui::Text("Refresh rate: "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i", refressRate);

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
		static bool active = false;
		ImGui::Checkbox("Active", &active);

		ImGui::Text("SDL Version: "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "TODO");

		ImGui::SameLine();
		ImGui::Text("CPUs: "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "TODO");

		ImGui::Text("System RAM: "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "TODO");

		ImGui::Text("Caps: "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "TODO");

		ImGui::SameLine();
		ImGui::Text("GPU: "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "TODO");

		ImGui::Text("Brand: "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "TODO");

		ImGui::Text("VRAM Budget: "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "TODO");

		ImGui::Text("VRAM Usage: "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "TODO");

		ImGui::Text("VRAM Available: "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "TODO");

		ImGui::Text("VRAM REserved: "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "TODO");
	}

	if (ImGui::CollapsingHeader("Camera"))
	{
		static bool active = false;
		ImGui::Checkbox("Active", &active);

		static float front[3] = { App->editorCamera->GetFront().ToFloatArray() };
		ImGui::InputFloat3("Front", front);

		static float up[3] = { App->editorCamera->GetUp().ToFloatArray() };
		ImGui::InputFloat3("Up", up);

		static float position[3] = { App->editorCamera->GetPosition().ToFloatArray() };
		ImGui::InputFloat3("Position", position);

		static float movementSpeed = App->editorCamera->GetMovementSpeed();
		ImGui::DragFloat("Mov Speed", &movementSpeed, 0.01f);

		static float rotationSpeed = App->editorCamera->GetRotationSpeed();
		ImGui::DragFloat("Rot Speed", &rotationSpeed, 0.005f);

		static float zoomSpeed = App->editorCamera->GetZoomSpeed();
		ImGui::DragFloat("Zoom Speed", &zoomSpeed, 0.005f);

		static bool frustum = false;
		ImGui::Checkbox("Frustum Culling", &frustum);

		static float nearPlane = App->editorCamera->GetNearPlane();
		ImGui::DragFloat("Near Plane", &nearPlane, 0.1f);

		static float farPlane = App->editorCamera->GetFarPlane();
		ImGui::DragFloat("Far Plane", &farPlane, 1.0f);

		static float fov = App->editorCamera->GetFOV();
		ImGui::DragFloat("Field of View", &fov, 1.0f);

		static float ascpectRatio = App->editorCamera->GetAspectRatio();
		ImGui::DragFloat("Aspect Ratio", &ascpectRatio, 0.01f);

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

	ImGui::End();
}
