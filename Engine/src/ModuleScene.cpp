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

update_status ModuleScene::Update()
{
	root->Update();
	return UPDATE_CONTINUE;
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
		child->Draw(program);
	}
}
