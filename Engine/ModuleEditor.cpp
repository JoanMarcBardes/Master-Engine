#include "ModuleEditor.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleWindow.h"
#include "SDL.h"
#include "GL/glew.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_sdl.h"
#include "ImGui/imgui_impl_opengl3.h"


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

	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer->GetContext());
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
	WindowHello();
	WindowConsole();
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

		ImGui::Text("Limit Framerate: %i",fps);

		/*char title[25];
		sprintf_s(title, 25, "Framerate %.1f", fps_log[fps_log.size() - 1]);
		ImGui::PlotHistogram("##framerate", &fps_log[0], fps_log.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));
		sprintf_s(title, 25, "Milliseconds %.1f", ms_log[ms_log.size() - 1]);
		ImGui::PlotHistogram("##framerate", &ms_log[0], ms_log.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));*/

	}

	ImGui::End();
}
