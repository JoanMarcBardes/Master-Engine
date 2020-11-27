#include "ModuleRenderExercise.h"
#include "ModuleEditorCamera.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleInput.h"
#include "ModuleWindow.h"
#include "Point.h"
#include "SDL.h"
#include "GL/glew.h"
#include "ModuleProgram.h"
#include "MathGeoLib/Math/float4x4.h"
#include "debugdraw.h"
#include "debug_draw.hpp"
#include "ModuleDebugDraw.h"
#include "ModuleTexture.h"
#include "ModuleModel.h"
#include "DebugLeaks.h"
#include <string>
#include <direct.h>
#include <crtdbg.h>

using namespace std;

std::string get_current_dir() {
	char buff[FILENAME_MAX]; //create string buffer to hold path
	_getcwd(buff, FILENAME_MAX);
	string current_dir(buff);
	return current_dir;
}

void ReplaceSlash(string& str)
{
	string oldStr = "\\";
	string newStr = "/";
	size_t index = 0;
	while (true) {
		index = str.find(oldStr, index);
		if (index == string::npos) break;

		str.replace(index, oldStr.length(), newStr);

		index += 3; //Advance index forward so the next iteration doesn't pick it up as well.
	}
}

void __stdcall OurOpenGLErrorFunction(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	const char* tmp_source = "", * tmp_type = "", * tmp_severity = "";
	switch (source) {
	case GL_DEBUG_SOURCE_API: tmp_source = "API"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM: tmp_source = "Window System"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: tmp_source = "Shader Compiler"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY: tmp_source = "Third Party"; break;
	case GL_DEBUG_SOURCE_APPLICATION: tmp_source = "Application"; break;
	case GL_DEBUG_SOURCE_OTHER: tmp_source = "Other"; break;
	};
	switch (type) {
	case GL_DEBUG_TYPE_ERROR: tmp_type = "Error"; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: tmp_type = "Deprecated Behaviour"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: tmp_type = "Undefined Behaviour"; break;
	case GL_DEBUG_TYPE_PORTABILITY: tmp_type = "Portability"; break;
	case GL_DEBUG_TYPE_PERFORMANCE: tmp_type = "Performance"; break;
	case GL_DEBUG_TYPE_MARKER: tmp_type = "Marker"; break;
	case GL_DEBUG_TYPE_PUSH_GROUP: tmp_type = "Push Group"; break;
	case GL_DEBUG_TYPE_POP_GROUP: tmp_type = "Pop Group"; break;
	case GL_DEBUG_TYPE_OTHER: tmp_type = "Other"; break;
	};
	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH: tmp_severity = "high"; break;
	case GL_DEBUG_SEVERITY_MEDIUM: tmp_severity = "medium"; break;
	case GL_DEBUG_SEVERITY_LOW: tmp_severity = "low"; break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: tmp_severity = "notification"; break;
	};
	//LOG("<Source:%s> <Type:%s> <Severity:%s> <ID:%d> <Message:%s>\n", tmp_source, tmp_type, tmp_severity, id, message);
}


ModuleRenderExercise::ModuleRenderExercise()
{
}

ModuleRenderExercise::~ModuleRenderExercise()
{
}

bool ModuleRenderExercise::Init()
{
	LOG("Creating ModuleRenderExercise context");

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4); // desired version
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); // we want a double buffer
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24); // we want to have a depth buffer with 24 bits
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8); // we want to have a stencil buffer with 8 bits

	// Create an OpenGL context associated with the window.
	_context = SDL_GL_CreateContext(App->window->window);
	GLenum err = glewInit();

#ifdef _DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(&OurOpenGLErrorFunction, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, true);
#endif

	SDL_EventState(SDL_DROPFILE, SDL_ENABLE);

	CreateTriangleVBO();
	CreateQuadVBO();
	LoadMeshes();

	unsigned idVertex = App->program->CompileShader (GL_VERTEX_SHADER, App->program->LoadShaderSource("default_vertex.glsl"));
	unsigned idFragment = App->program->CompileShader(GL_FRAGMENT_SHADER, App->program->LoadShaderSource("default_fragment.glsl"));

	_program = App->program->CreateProgram(idVertex, idFragment);

	glEnable(GL_DEPTH_TEST); // Enable depth test
	glEnable(GL_CULL_FACE); // Enable cull backward faces
	glFrontFace(GL_CCW); // Front faces will be counter clockwise

	// Current directory
	_currentDir = get_current_dir() + "\\";
	ReplaceSlash(_currentDir);

	return true;
}

update_status ModuleRenderExercise::PreUpdate()
{
	int w, h;
	SDL_GetWindowSize(App->window->window, &w, &h);
	glViewport(0, 0, w, h);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	return UPDATE_CONTINUE;
}

// Called every draw update
update_status ModuleRenderExercise::Update()
{
	DropFile();
	Draw();

	return UPDATE_CONTINUE;
}

update_status ModuleRenderExercise::PostUpdate()
{
	SDL_GL_SwapWindow(App->window->window);
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRenderExercise::CleanUp()
{
	LOG("Destroying ModuleRenderExercise");

	DestroyVBO(_vboTriangle);
	DestroyVBO(_vboQuad);
	SDL_GL_DeleteContext(_context);

	return true;
}

void ModuleRenderExercise::WindowResized(unsigned width, unsigned height)
{
	App->editorCamera->WindowResized(width, height);
	SDL_SetWindowSize(App->window->window, width, height);
}


// This function must be called one time at creation of vertex buffer
void ModuleRenderExercise::CreateTriangleVBO()
{
	float buffer_data[] = {
		2.0f, 0.0f, 0.0f, // ← v0 pos
		4.0f, 0.0f, 0.0f, // ← v1 pos
		3.0f, 2.0f, 0.0f, // ← v2 pos
	};

	glGenBuffers(1, &_vboTriangle);
	glBindBuffer(GL_ARRAY_BUFFER, _vboTriangle); // set vbo active
	glBufferData(GL_ARRAY_BUFFER, sizeof(buffer_data), buffer_data, GL_STATIC_DRAW);
}

void ModuleRenderExercise::CreateQuadVBO()
{
	float buffer_data[] = {
		-1.0f, -1.0f, 0.0f, // ← v0 pos
		1.0f, -1.0f, 0.0f, // ← v1 pos
		-1.0f, 1.0f, 0.0f, // ← v2 pos

		-1.0f, 1.0f, 0.0f, // ← v2 pos
		1.0f, -1.0f, 0.0f, // ← v1 pos
		1.0f, 1.0f, 0.0f, // ← v3 pos

		0.0f, 0.0f, // ← v0 texcoord
		1.0f, 0.0f, // ← v1 texcoord
		0.0f, 1.0f, // ← v2 texcoord

		0.0f, 1.0f, // ← v2 texcoord
		1.0f, 0.0f, // ← v1 texcoord
		1.0f, 1.0f // ← v3 texcoord
	};

	glGenBuffers(1, &_vboQuad);
	glBindBuffer(GL_ARRAY_BUFFER, _vboQuad); // set vbo active
	glBufferData(GL_ARRAY_BUFFER, sizeof(buffer_data), buffer_data, GL_STATIC_DRAW);
}

void ModuleRenderExercise::LoadMeshes()
{
	//App->model->Load("BakerHouse.fbx");
	App->model->Load("Models/BakerHouse/BakerHouse.fbx");
	//App->model->Load("Models/backpack/backpack.obj");

	//App->model->Load("Models/AmongUs/AmongUs.fbx");
	//App->model->Load("Models/aug/aug.fbx");
	//App->model->Load("Models/BurntCroissant/BurntCroissant.fbx");
	//App->model->Load("Models/crow/crow.fbx");
	//App->model->Load("Models/fox/fox.fbx");
	//App->model->Load("Models/herschel_bag/herschel_bag.fbx");
	//App->model->Load("Models/WoodGirl/WoodGirl.fbx");
}


// This function must be called one time at destruction of vertex buffer
void ModuleRenderExercise::DestroyVBO(unsigned vbo)
{
	glDeleteBuffers(1, &vbo);
}


void ModuleRenderExercise::Draw()
{
	float4x4 model = float4x4::identity;
	float4x4 view = App->editorCamera->GetViewMatrix();
	float4x4 proj = App->editorCamera->GetProjection();
	int height = App->window->GetHeight();
	int width = App->window->GetWidth();

	App->debugDraw->Draw(view, proj, width, height);

	glUseProgram(_program);
	//DrawQuad(proj, view);
	//DrawTriangle(proj, view);
	DrawMesh(proj, view, model);
}

// This function must be called each frame for drawing the triangle
void ModuleRenderExercise::DrawTriangle(const float4x4& proj, const float4x4& view)
{
	float4x4 model = float4x4::identity;

	glBindBuffer(GL_ARRAY_BUFFER, _vboTriangle);
	glEnableVertexAttribArray(0);

	// size = 3 float per vertex
	// stride = 0 is equivalent to stride = sizeof(float)*3
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glUniformMatrix4fv(glGetUniformLocation(_program, "model"), 1, GL_TRUE, &model[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(_program, "view"), 1, GL_TRUE, (const float*)&view);
	glUniformMatrix4fv(glGetUniformLocation(_program, "proj"), 1, GL_TRUE, (const float*)&proj);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float) * 3 * 3));

	glActiveTexture(GL_TEXTURE0);

	// 1 triangle to draw = 3 vertices
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

void ModuleRenderExercise::DrawQuad(const float4x4& proj, const float4x4& view)
{
	float4x4 model = float4x4::identity;

	glBindBuffer(GL_ARRAY_BUFFER, _vboQuad);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glUniformMatrix4fv(glGetUniformLocation(_program, "model"), 1, GL_TRUE, &model[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(_program, "view"), 1, GL_TRUE, (const float*) &view);
	glUniformMatrix4fv(glGetUniformLocation(_program, "proj"), 1, GL_TRUE, (const float*) &proj);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float) * 6 * 3));
	
	glActiveTexture(GL_TEXTURE0);

	//glBindTexture(GL_TEXTURE_2D, App->texture->GetTexture());
	glUniform1i(glGetUniformLocation(_program, "mytexture"), 0);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void ModuleRenderExercise::DrawMesh(const float4x4& proj, const float4x4& view, const float4x4& model)
{
	App->model->DrawMeshes(_program, proj, view, model);
}

void ModuleRenderExercise::DropFile()
{
	SDL_Event sdlEvent;
	while (SDL_PollEvent(&sdlEvent) != 0)
	{
		switch (sdlEvent.type) {
			case (SDL_DROPFILE):      // In case if dropped file
				char* dropped_filedir = sdlEvent.drop.file;
				string s(dropped_filedir);
				ReplaceSlash(s);
				if (s.find(".fbx") < s.length())
				{
					int start_position_to_erase = s.find(_currentDir);
					s.erase(start_position_to_erase, _currentDir.size());
					LOG( ("Loading " + s).c_str());
					App->model->Load(s.c_str());
				}
				else
				{
					LOG( (s +" its not a file .fbx").c_str() );
				}
				SDL_free(dropped_filedir);    // Free dropped_filedir memory
				break;		
		}
	}
}