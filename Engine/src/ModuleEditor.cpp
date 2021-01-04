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
#include "EditorComponent.h"
#include "EditorConsole.h"
#include "EditorMainMenu.h"
#include "EditorConfig.h"
#include "EditorAbout.h"
#include "EditorPlay.h"
#include "EditorViewport.h"
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

ModuleEditor::ModuleEditor()
{
	EditorConsole* console = new EditorConsole();
	EditorMainMenu* mainMenu = new EditorMainMenu(this);
	EditorConfig* config = new EditorConfig();
	EditorAbout* about = new EditorAbout();
	EditorViewport* viewport = new EditorViewport();
	EditorPlay* play = new EditorPlay();
	components.push_back(mainMenu);
	components.push_back(console);
	components.push_back(config);
	components.push_back(about);
	components.push_back(viewport);
	components.push_back(play);
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
	SDL_GetWindowSize(App->window->window, &w, &h);

	bool quit = false;

	update_status status = UPDATE_CONTINUE;

	std::vector<EditorComponent*>::iterator it;
	for (it = components.begin(); it != components.end(); ++it)
	{
		switch ((*it)->GetType()) 
		{
		case 1:
			(*it)->Draw(w, h);
			quit = (*it)->IsQuitting();
			if (quit) status = UPDATE_STOP;
			menuSize = (*it)->GetMenuSize();
			break;
		case 2:
			if (showWindowConsole) 
			{
				(*it)->SetMenuSize(menuSize);
				(*it)->SetActive(&showWindowConsole);
				(*it)->SetItems(Items);
				SDL_GetWindowSize(App->window->window, &w, &h);

				(*it)->Draw(w, h);
			}
			break;
		case 3:
			if (showWindowConfiguration)
			{
				(*it)->SetMenuSize(menuSize);
				(*it)->Draw(w, h);
			}
			break;
		case 4:
			if (showAbout)
			{
				(*it)->SetMenuSize(menuSize);
				(*it)->Draw(w, h);
			}
			break;
		case 5:
			if (showWindowViewport)
			{
				(*it)->SetMenuSize(menuSize);
				(*it)->Draw(w, h);
			}
			break;
		case 6:
			if (showWindowPlayStopStep)
			{
				(*it)->SetMenuSize(menuSize);
				(*it)->Draw(w, h);
			}
			break;
		}
	}

	if (showWindowGameObjectHierarchy) WindowGameObjectHierarchy(&showWindowGameObjectHierarchy);
	if (showWindowInspector) WindowInspector(&showWindowInspector);

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

void ModuleEditor::WindowGameObjectHierarchy(bool* p_open)
{
	SDL_GetWindowSize(App->window->window, &w, &h);
	float fW = (float)w;
	float fH = (float)h;

	fH = (float)h / 2 + menuSize;

	fW = (float)w;
	fW = fW - (fW / 5);

	ImVec2 pos2(fW, fH);
	ImGui::SetNextWindowPos(pos2);

	fW = (float)w;
	fH = (float)h;

	fW = fW / 5;
	fH = fH / 2 - menuSize;
	ImVec2 size(fW, fH);

	if (!ImGui::Begin("GameObjectHierarchy", p_open/*, ImGuiWindowFlags_AlwaysUseWindowPadding*/))
	{
		ImGui::End();
		return;
	}

	if (ImGui::TreeNode("Root"))
	{
		GameObject* root = App->scene->GetRoot();

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TREENODE"))
			{
				LOG(root->name.c_str());
				selected->SetParent(root);
			}
			ImGui::EndDragDropTarget();
		}

		TreeChilds(root);
		ImGui::TreePop();
	}

	ImGui::SetWindowSize(size);
	ImGui::End();
}

void ModuleEditor::TreeChilds(GameObject* parent)
{
	static ImGuiTreeNodeFlags baseFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
	std::vector<GameObject*> childs = parent->GetChilds();
	for each (GameObject * child in childs)
	{
		ImGuiTreeNodeFlags nodeFlags = baseFlags;
		if (selected && child->GetID() == selected->GetID())
			nodeFlags |= ImGuiTreeNodeFlags_Selected;

		if (child->GetNumChilds() == 0) 
			nodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet

		bool node_open = ImGui::TreeNodeEx(child->name.c_str(), nodeFlags);

		if (ImGui::GetIO().KeyCtrl && ImGui::IsItemClicked())
			selected = nullptr;
		else if (ImGui::IsItemClicked())
			selected = child;

		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("_TREENODE", NULL, 0);
			ImGui::Text("%s", child->name.c_str());
			ImGui::EndDragDropSource();
		}
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TREENODE"))
			{
				bool isLeaf = child->GetNumChilds() == 0;
				LOG(child->name.c_str());
				selected->SetParent(child);

				if (isLeaf) // is target gameObject is leaf need to skipe the TreePop
					return;
			}
			ImGui::EndDragDropTarget();
		}

		if (child->GetNumChilds() > 0)
		{
			if (node_open)
			{
				TreeChilds(child);
				ImGui::TreePop();
			}
		}
		
	}
	
}

void ModuleEditor::WindowInspector(bool* p_open)
{
	SDL_GetWindowSize(App->window->window, &w, &h);
	float fW = (float)w;
	float fH = (float)h;

	fW = (float)w;
	fW = fW - (fW / 5);

	ImVec2 pos2(fW, menuSize);
	ImGui::SetNextWindowPos(pos2); 
	
	fW = (float)w;
	fH = (float)h;

	fW = fW / 5;
	fH = fH / 2;
	ImVec2 size(fW, fH);

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

	ImGui::SetWindowSize(size);
	ImGui::End();
}
