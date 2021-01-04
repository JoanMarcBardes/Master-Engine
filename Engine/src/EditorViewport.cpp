#include "EditorViewport.h"
#include "Application.h"
#include "ModuleRenderExercise.h"

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