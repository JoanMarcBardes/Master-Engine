#include "EditorViewport.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleRenderExercise.h"
#include "Libraries/ImGui/imgui.h"
#include "Libraries/ImGuizmo-master/ImGuizmo.h"

void EditorViewport::Draw(int w, int h)
{
	float fW = (float)w;
	float fH = (float)h;

	fW = fW / 5;
	fH = fH - (fH / 4);
	ImVec2 size(fW, fH);

	fW = (float)w;
	fW = fW / 5;

	ImVec2 pos0(fW, menuSize);
	ImVec2 sizeView(fW * 3, size.y);

	ImGui::SetNextWindowPos(pos0);
	ImGui::Begin("Viewport", 0, ImGuiWindowFlags_NoMove);

	ImGui::Image((ImTextureID)App->renderExercise->GetRenderText(), { sizeView.x * 0.985f, sizeView.y * 0.95f }, { 0,1 }, { 1,0 });

	if (ImGui::IsWindowFocused()) viewportFocused = true;
	else viewportFocused = false;

	ImGui::SetWindowSize(sizeView);
	ImGui::End();

}

void EditorViewport::DrawGuizmo(Camera* camera, GameObject* go)
{
	ImGuizmo::BeginFrame();
	ImGuizmo::Enable(true);

	static bool draw_guizmo = false;
	static ImGuizmo::OPERATION current_operation(ImGuizmo::TRANSLATE);
	static ImGuizmo::MODE current_mode(ImGuizmo::WORLD);

	if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_IDLE) {
		if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_DOWN) //Selected
			draw_guizmo = false;

		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN) { //Translate
			draw_guizmo = true;
			current_operation = ImGuizmo::TRANSLATE;
		}
		if (App->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN) { //Rotate
			draw_guizmo = true;
			current_operation = ImGuizmo::ROTATE;
		}
		if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN) { //Scale
			draw_guizmo = true;
			current_operation = ImGuizmo::SCALE;
		}
		if (App->input->GetKey(SDL_SCANCODE_T) == KEY_DOWN) { //Change global to world and backwards
			if (current_mode == ImGuizmo::MODE::WORLD)
				current_mode = ImGuizmo::MODE::LOCAL;
			else
				current_mode = ImGuizmo::MODE::WORLD;
		}
	}


	if (draw_guizmo) 
	{
		float4x4 view = camera->GetViewMatrix();
		float4x4 proj = camera->GetProjection();

		Transform* transf = (Transform*)go->GetComponent(Component::Type::Transform);
		float4x4 model = transf->GetTransformGlobal();
		model.Transpose();

		ImGuiIO& io = ImGui::GetIO();
		ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
		ImGuizmo::Manipulate((float*)view.v, (float*)proj.v, current_operation, ImGuizmo::MODE::WORLD, (float*)&model);

		if (ImGuizmo::IsUsing()) {
			//Put the values of the matrix in each corresponding vector/quaternion
			model.Transpose();
			GameObject* parent = go->GetParent();

			//If the GameObject does have a parent, we have to recalculate the local matrix (root_node->parent is always null!!)
			if (parent->GetParent() != nullptr) {
				Transform* transfParent = (Transform*)parent->GetComponent(Component::Type::Transform);
				model = transfParent->GetTransformGlobal().Inverted() * model;
			}

			if (current_operation != ImGuizmo::SCALE) {
				model.Decompose(transf->GetPosition(), transf->GetRotation(), transf->GetScale());

				if (current_operation == ImGuizmo::ROTATE) {

				}
					//SetRotationFromQuat();
			}
			else {
				//current_mode = ImGuizmo::MODE::LOCAL;
				float3 disposable_pos;
				Quat disposable_rot;
				model.Decompose(disposable_pos, disposable_rot, transf->GetScale());
			}
		}
	}

}