#include "EditorAbout.h"

void EditorAbout::Draw(int w, int h)
{

	if (!ImGui::Begin("About", active))
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