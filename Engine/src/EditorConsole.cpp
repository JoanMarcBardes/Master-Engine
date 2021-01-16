#include "EditorConsole.h"
#include "Application.h"
#include "ModuleEditor.h"


void EditorConsole::Draw(int w, int h)
{
	float fW = (float)w;
	float fH = (float)h;

	fH = fH - (fH / 4);

	ImVec2 pos3(0.0f, fH + menuSize);
	ImGui::SetNextWindowPos(pos3);

	fW = fW - (fW / 5);
	fH = (float)h;
	fH = fH / 4;

	ImVec2 size = { fW,fH - menuSize };
	//ImGui::SetNextWindowSize(size, ImGuiCond_FirstUseEver);

	if (!ImGui::Begin("Console", active, ImGuiWindowFlags_NoMove))
	{
		ImGui::End();
		return;
	}

	if (ImGui::SmallButton("Clear")) { App->editor->ClearLog(); };

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
		else if (strstr(item, "[warning]")) { color = ImVec4(1.0f, 1.0f, 0.4f, 1.0f); has_color = true; }
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
	ImGui::SetWindowSize(size);
	ImGui::End();
}