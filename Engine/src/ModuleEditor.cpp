#include "ModuleEditor.h"
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
#include "Time.h"
#include "SDL.h"
#include "GL/glew.h"
#include "Libraries/ImGui/imgui.h"
#include "Libraries/ImGui/imgui_impl_sdl.h"
#include "Libraries/ImGui/imgui_impl_opengl3.h"
#include "Libraries/MathGeoLib/MathGeoLib.h"
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
	fpsLog.clear();
	msLog.clear();
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
	if (showWindowGameObjectHierarchy) WindowGameObjectHierarchy(&showWindowGameObjectHierarchy);
	if (showWindowInspector) WindowInspector(&showWindowInspector);
	if (showWindowPlayStopStep) WindowPlayStopStep(&showWindowPlayStopStep);

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	return status;
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
		char appName[50];
		strcpy_s(appName, 50, App->GetTitle());
		if (ImGui::InputText("Project Name", appName, IM_ARRAYSIZE(appName), ImGuiInputTextFlags_EnterReturnsTrue))
			App->SetTitle(appName);

		char organization[50];
		strcpy_s(organization, 50, App->GetOrganizatio());
		if (ImGui::InputText("Organization Name", organization, IM_ARRAYSIZE(organization), ImGuiInputTextFlags_EnterReturnsTrue))
			App->SetOrganizatio(organization);

		int fpsMax = App->GetFpsMax();
		if(ImGui::SliderInt("fps Max", &fpsMax, 1, 60))
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

	ImGui::End();	
}

//10. There must be a general menu with option to quit, visit the github page, and get info about the
//		engine(“About”).It should also be able to turn on / off editor windows
update_status ModuleEditor::MainMenuBar()
{
	update_status status = UPDATE_CONTINUE;
	ImGui::BeginMainMenuBar();
	if (ImGui::BeginMenu("Help"))
	{		
		if (ImGui::MenuItem("Github repository"))
		{
			RequestBrowser("https://github.com/JoanMarcBardes/Master-Engine");
		}
		ImGui::MenuItem("About", NULL, &showAbout);

		if (ImGui::BeginMenu("Windows"))
		{
			ImGui::MenuItem("bWindowConsole", "", &showWindowConsole);
			ImGui::MenuItem("WindowConfiguration", "", &showWindowConfiguration);
			ImGui::MenuItem("WindowGameObjectHierarchy", "", &showWindowGameObjectHierarchy);
			ImGui::MenuItem("WindowInspector", "", &showWindowInspector);
			ImGui::MenuItem("WindowPlayStopStep", "", &showWindowPlayStopStep);
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
	if (!ImGui::Begin("About", p_open, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::End();
		return;
	}

	ImGui::Text(
		"Joan-Marc-Engine\n"
		"Joan Marc Engine it's an engine developed during the Master in Advanced Programming for AAA Video Games.\n"
		"You can found the poryect on : https://github.com/JoanMarcBardes/Master-Engine \n\n"

		"Authors\n"
		"Joan Marc Bardés Vera, guithub account : https://github.com/JoanMarcBardes \n\n"

		"License\n"
		"This project is under the MIT License - see the[LICENSE.md] file for details\n\n"

		"Guide how to use the engine\n\n"

		"Camera controls:\n"
		" * Q / E go up / down.\n"
		" * W / S move forward and backward.\n"
		" * A / D move left and right.\n"
		" * Up / Down and Left / Right arrows for eotate the camera.\n"
		" * Right mouse click movement.\n"
		" * Alt + Right free look around.\n"
		" * Mouse wheel for zoom inand out.\n"
		" * Alt + Left click for orbit the object.\n"
		" * F for focus the camera around the geometry.\n"
		" * Holding SHIFT duplicates movement speed.\n"
		" * O for enable / disable orbit the object with the movement.\n\n"

		"Drag to window engine a.fbx file for replace the model.\n"
		"Drag to window engine a.png / jpg / dds file for replace the texture.\n\n"

		"In configuration window, can configureand visualise several options of the engine.\n"
	);

	ImGui::End();
}

void ModuleEditor::RequestBrowser(const char* url)
{
	ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWDEFAULT);
}


void ModuleEditor::WindowGameObjectHierarchy(bool* p_open)
{
	if (!ImGui::Begin("GameObjectHierarchy", p_open/*, ImGuiWindowFlags_AlwaysUseWindowPadding*/))
	{
		ImGui::End();
		return;
	}

	if (ImGui::TreeNode("Root"))
	{
		GameObject* root = App->scene->GetRoot();
		TreeChilds(root);
		ImGui::TreePop();
	}

	ImGui::End();
}

void ModuleEditor::TreeChilds(GameObject* parent)
{
	static ImGuiTreeNodeFlags baseFlags;
	std::vector<GameObject*> childs = parent->GetChilds();
	for each (GameObject * child in childs)
	{
		if (child->GetNumChilds() > 0)
		{
			baseFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
			bool node_open = ImGui::TreeNodeEx(child->name.c_str(), baseFlags);			
			
			if (ImGui::IsItemClicked())
				selected = child;		

			if (node_open)
			{
				TreeChilds(child);
				ImGui::TreePop();
			}
		}
		else
		{
			ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet
			ImGui::TreeNodeEx(child->name.c_str(), nodeFlags);

			if (ImGui::IsItemClicked())
				selected = child;
		}		
		
	}
}

void ModuleEditor::WindowInspector(bool* p_open)
{
	if (!ImGui::Begin("Inspector", p_open))
	{
		ImGui::End();
		return;
	}

	if (selected)
	{
		bool active = selected->IsActive();
		if (ImGui::Checkbox("Active", &active))
			selected->SetActive(active);

		ImGui::SameLine();
		char goName[50];
		strcpy_s(goName, 50, selected->name.c_str());
		ImGuiInputTextFlags textFlags = ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue;
		if (ImGui::InputText("###", goName, 50, textFlags))
			selected->name = goName;


		ImGui::SameLine();
		ImGui::PushID(0);
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(1.0f, 0.6f, 0.6f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(1.0f, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(1.0f, 0.8f, 0.8f));
		if (ImGui::SmallButton("Delete"))
		{
			delete selected;
			selected = nullptr;
			ImGui::PopStyleColor(3);
			ImGui::PopID();
			ImGui::End();
			return;
		}
		ImGui::PopStyleColor(3);
		ImGui::PopID();

		ImGui::Separator();

		Transform* transform = (Transform*)selected->GetComponent(Component::Type::Transform);
		if (ImGui::CollapsingHeader("Transform"))
		{
			float3 position = transform->GetPosition();
			float3 rotationEuler = transform->GetRotationEuler();
			float3 scale = transform->GetScale();

			if(ImGui::DragFloat3("Position", position.ptr(), 0.1f))
				transform->SetPosition(position);
			if(ImGui::DragFloat3("Rotate", rotationEuler.ptr(), 0.1f))
				transform->SetRotationEuler(rotationEuler);
			if(ImGui::DragFloat3("Scale", scale.ptr(), 0.1f))
				transform->SetScale(scale);

			ImGui::Separator();

			float4x4 globalTransfrom= transform->GetTransformGlobal();
			Quat rotation;
			globalTransfrom.Decompose(position, rotation, scale);
			rotationEuler = rotation.ToEulerXYZ() * RADTODEG;

			if (ImGui::DragFloat3("PositionGlobal", position.ptr(), 0.1f))
				transform->SetPosition(position);
			if (ImGui::DragFloat3("RotateGlobal", rotationEuler.ptr(), 0.1f))
				transform->SetRotationEuler(rotationEuler);
			if (ImGui::DragFloat3("ScaleGlobal", scale.ptr(), 0.1f))
				transform->SetScale(scale);
		}
		
		Mesh* mesh = (Mesh*)selected->GetComponent(Component::Type::Mesh);
		if (mesh && ImGui::CollapsingHeader("Mesh"))
		{
			ImGui::Text("Num vertices: "); ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i", mesh->GetNumVertices());

			ImGui::Text("Num indices: "); ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i", mesh->GetNumIndices());
		}

		Material* material = (Material*)selected->GetComponent(Component::Type::Material);
		if (material && ImGui::CollapsingHeader("Material"))
		{
			ImGui::Text("Texture");
			std::vector<std::string> paths = material->GetPaths();
			for (int i = 0; i < paths.size(); ++i)
			{
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", paths[i].c_str());
			}

			std::vector<unsigned int> textures = material->GetTextures();
			float my_tex_w = 150;
			float my_tex_h = 150;
			for (int j = 0; j < textures.size(); ++j)
			{
				if (j != 0) ImGui::SameLine();
				ImVec2 pos = ImGui::GetCursorScreenPos();
				ImVec2 uv_min = ImVec2(0.0f, 0.0f);                 // Top-left
				ImVec2 uv_max = ImVec2(1.0f, 1.0f);                 // Lower-right
				ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
				ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f); // 50% opaque white
				ImGui::Image((ImTextureID)textures[j], ImVec2(my_tex_w, my_tex_h), uv_min, uv_max, tint_col, border_col);
			}
		}
	}
	else
	{
		ImGui::Text("Select a GameObject");
	}

	ImGui::End();
}

void ModuleEditor::WindowPlayStopStep(bool* p_open)
{
	if (!ImGui::Begin("PlayStopStep", p_open)) 
	{
		ImGui::End();
		return;
	}

	std::string name = Time::running ? "Stop" : "Play";
	if (ImGui::Button(name.c_str()))
	{
		Time::running ? Time::Stop() : Time::Play();
	}
	ImGui::SameLine();
	std::string name2 = Time::paused ? "Continue" : "Pause";
	if (ImGui::Button(name2.c_str()))
	{
		Time::paused ? Time::Continue() : Time::Pause();
	}
	ImGui::SameLine();
	//ImGuiButtonFlags flag = Time::paused ? 1 << 15 : 1 << 14; // ImGuiButtonFlags_AlignTextBaseLine or ImGuiButtonFlags_Disabled
	if (ImGui::Button("Step"))
	{
		Time::DoStep();
	}

	ImGui::End();
}
