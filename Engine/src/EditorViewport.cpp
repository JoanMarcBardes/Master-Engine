#include "EditorViewport.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleRenderExercise.h"
#include "ModuleScene.h"
#include "ModuleEditor.h"
#include "Libraries/ImGui/imgui.h"
#include "Libraries/ImGuizmo-master/ImGuizmo.h"

void EditorViewport::Draw(int w, int h)
{
	fW = (float)w;
	fH = (float)h;

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

	GameObject* selected = App->editor->GetSelectedGameObject();
	if (selected) {
		//DrawGuizmo((Camera*)App->scene->GetMainCamera()->GetComponent(Component::Type::Camera), selected);
	}

	ImGui::SetWindowSize(sizeView);
	ImGui::End();

}

void EditorViewport::DrawGuizmo(Camera* camera, GameObject* go)
{
	ImGuizmo::Enable(true);

	static bool draw_guizmo = true;
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

		ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, float(fW), float(fH));
		ImGuizmo::Manipulate((const float*)&view, (const float*)&proj, current_operation, current_mode, (float*)&model);

		if (ImGuizmo::IsUsing()) {
			model.Transpose();
			GameObject* parent = go->GetParent();

			if (parent->GetParent() != nullptr) {
				Transform* transfParent = (Transform*)parent->GetComponent(Component::Type::Transform);
				model = transfParent->GetTransformGlobal().Inverted() * model;
			}

			if (current_operation != ImGuizmo::SCALE) {
				model.Decompose(transf->GetPosition(), transf->GetRotation(), transf->GetScale());

				if (current_operation == ImGuizmo::ROTATE) {

				}
			}
			else {
				float3 disposable_pos;
				Quat disposable_rot;
				model.Decompose(disposable_pos, disposable_rot, transf->GetScale());
			}
		}
	}

}