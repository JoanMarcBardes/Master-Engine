#include "EditorPlay.h"

void EditorPlay::Draw(int w, int h)
{
	float fW = (float)w;

	fW = fW / 5;

	ImVec2 pos0(fW, menuSize *2);

	ImGui::SetNextWindowPos(pos0);

	if (!ImGui::Begin("PlayStopStep", active))
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