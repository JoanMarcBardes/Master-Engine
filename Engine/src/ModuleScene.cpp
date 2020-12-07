#include "ModuleScene.h"
#include "Application.h"
#include "ModuleProgram.h"
#include "ModuleEditorCamera.h"
#include "GL/glew.h"
#include <vector>

ModuleScene::ModuleScene()
{
	root = new GameObject(nullptr, "Root");
}

bool ModuleScene::Init()
{
	char* sourceVertex = App->program->LoadShaderSource("Shaders/Phong_VertexShader.glsl"); //vertexShader //Phong_VertexShader
	char* sourceFragment = App->program->LoadShaderSource("Shaders/Phong_FragmentShader.glsl"); //fragmentShader //Phong_FragmentShader
	idVertex = App->program->CompileShader(GL_VERTEX_SHADER, sourceVertex);
	idFragment = App->program->CompileShader(GL_FRAGMENT_SHADER, sourceFragment);
	free(sourceVertex);
	free(sourceFragment);

	return true;
}

GameObject* ModuleScene::CreateGameObject(const char* name, GameObject* parent)
{
	unsigned program = App->program->CreateProgram(idVertex, idFragment);
	GameObject* newGameObject = new GameObject(parent ? parent : root, name);
	newGameObject->SetProgram(program);
	return newGameObject;
}

void ModuleScene::Draw(unsigned program)
{
	std::vector<GameObject*> childs = root->GetChilds();
	for each (GameObject* child in childs)
	{
		/*unsigned program = child->GetProgram();
		float4x4 model = float4x4::identity;
		float4x4 view = App->editorCamera->GetViewMatrix();
		float4x4 proj = App->editorCamera->GetProjection();
		
		glUseProgram(program);
		glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, (const float*)&model);
		glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_TRUE, (const float*)&view);
		glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_TRUE, (const float*)&proj);

		float3 camera_pos = App->editorCamera->GetPosition();
		float3 light_dir = float3(1, 0, 0);
		float4 light_color = float4(1, 1, 1, 1);
		float4 ambient_color = float4(0.1f, 0.1f, 0.1f, 1.0f);
		float Ks = 0.7f;
		float Kd = 0.6f;
		int shininess = 70;

		glUniform3f(glGetUniformLocation(program, "camera_pos"), camera_pos.x, camera_pos.y, camera_pos.z);
		glUniform3f(glGetUniformLocation(program, "light_dir"), light_dir.x, light_dir.y, light_dir.z);
		glUniform4f(glGetUniformLocation(program, "light_color"), light_color.x, light_color.y, light_color.z, light_color.w);
		glUniform4f(glGetUniformLocation(program, "ambient_color"), ambient_color.x, ambient_color.y, ambient_color.z, ambient_color.w);
		glUniform1f(glGetUniformLocation(program, "Ks"), Ks);
		glUniform1f(glGetUniformLocation(program, "Kd"), Kd);
		glUniform1i(glGetUniformLocation(program, "shininess"), shininess);*/

		child->Draw(program);
		
		//glUseProgram(0);
		//glBindVertexArray(0);
	}
}
