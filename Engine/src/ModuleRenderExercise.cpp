#include "ModuleRenderExercise.h"
#include "ModuleEditorCamera.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleWindow.h"
#include "Point.h"
#include "SDL.h"
#include "GL/glew.h"
#include "ModuleProgram.h"
#include "Libraries/MathGeoLib/Math/float4x4.h"
#include "debugdraw.h"
#include "debug_draw.hpp"
#include "ModuleDebugDraw.h"
#include "ModuleTexture.h"
#include "ModuleModel.h"
#include "DebugLeaks.h"
#include "ModuleScene.h"
#include <string>
#include <crtdbg.h>
#include <direct.h>

using namespace std;

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


void ReplaceSlashS(string& str)
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
ModuleRenderExercise::ModuleRenderExercise()
{
}

ModuleRenderExercise::~ModuleRenderExercise()
{
}

bool ModuleRenderExercise::Init()
{
	LOG("Creating ModuleRenderExercise context");

	// Create an OpenGL context associated with the window.
	_context = SDL_GL_CreateContext(App->window->window);
	GLenum err = glewInit();

	if (SDL_GL_SetSwapInterval(VSYNC) < 0)
		LOG("Warning: Unable to set VSync! SDL Error: %s", SDL_GetError());

#ifdef _DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(&OurOpenGLErrorFunction, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, true);
#endif

	LoadMeshes();

	char* sourceVertex = App->program->LoadShaderSource("Shaders/Phong_BRDF_VS.glsl"); //Phong_BRDF_VS //Phong_VertexShader
	char* sourceFragment = App->program->LoadShaderSource("Shaders/Phong_BRDF_PS.glsl"); //Phong_BRDF_PS //Phong_FragmentShader
	unsigned idVertex = App->program->CompileShader (GL_VERTEX_SHADER, sourceVertex);
	unsigned idFragment = App->program->CompileShader(GL_FRAGMENT_SHADER, sourceFragment);
	free(sourceVertex);
	free(sourceFragment);

	_program = App->program->CreateProgram(idVertex, idFragment);

	SetGlEnable(enableDepthTest, GL_DEPTH_TEST);
	SetGlEnable(enableCullFace, GL_CULL_FACE);
	SetGlEnable(enableAlphaTest, GL_ALPHA_TEST);
	glFrontFace(GL_CCW); // Front faces will be counter clockwise
	
	return true;
}

update_status ModuleRenderExercise::PreUpdate()
{
	int w, h;
	SDL_GetWindowSize(App->window->window, &w, &h);
	glViewport(0, 0, w, h);
	glClearColor(background.x, background.y, background.z, background.w);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	return UPDATE_CONTINUE;
}

// Called every draw update
update_status ModuleRenderExercise::Update()
{
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

	SDL_GL_DeleteContext(_context);
	glDeleteProgram(_program);

	return true;
}

void ModuleRenderExercise::WindowResized(unsigned width, unsigned height)
{
	App->editorCamera->WindowResized(width, height);
	SDL_SetWindowSize(App->window->window, width, height);
}

void ModuleRenderExercise::LoadMeshes()
{
	//App->model->Import("Models/BakerHouse/BakerHouse.fbx");
	App->scene->Load();
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

	std::vector<Camera*> allCameras = App->editorCamera->GetAllCameras();
	for each (Camera* cam in allCameras)
	{
		App->debugDraw->DrawCamera(cam->frustum.ViewProjMatrix().Inverted());
	}
	App->debugDraw->Draw(view, proj, width, height); //draw axisTriad and xzSquareGrid


	glUseProgram(_program);
	DrawMesh(proj, view, model);
}

void ModuleRenderExercise::DrawMesh(const float4x4& proj, const float4x4& view, const float4x4& model)
{
	//glUniformMatrix4fv(glGetUniformLocation(_program, "model"), 1, GL_TRUE, (const float*)&model);
	glUniformMatrix4fv(glGetUniformLocation(_program, "view"), 1, GL_TRUE, (const float*)&view);
	glUniformMatrix4fv(glGetUniformLocation(_program, "proj"), 1, GL_TRUE, (const float*)&proj);

	float3 camera_pos = App->editorCamera->GetPosition();

	glUniform3f(glGetUniformLocation(_program, "camera_pos"), camera_pos.x, camera_pos.y, camera_pos.z);
	glUniform3f(glGetUniformLocation(_program, "light_dir"), light_dir.x, light_dir.y, light_dir.z);
	glUniform3f(glGetUniformLocation(_program, "light_color"), light_color.x, light_color.y, light_color.z);

	glUniform3f(glGetUniformLocation(_program, "ambient_color"), ambient_color.x, ambient_color.y, ambient_color.z);
	glUniform1i(glGetUniformLocation(_program, "shininess"), shininess);
	glUniform3f(glGetUniformLocation(_program, "diffuse_color"), diffuse_color.x, diffuse_color.y, diffuse_color.z);
	glUniform3f(glGetUniformLocation(_program, "specular_color"), specular_color.x, specular_color.y, specular_color.z);

	glUniform1i(glGetUniformLocation(_program, "has_diffuse_map"), has_diffuse_map);
	glUniform1i(glGetUniformLocation(_program, "has_specular_map"), has_specular_map);
	glUniform1i(glGetUniformLocation(_program, "shininess_alpha"), shininess_alpha);

	//App->model->DrawMeshes(_program);
	App->scene->Draw(_program);
}

void ModuleRenderExercise::SetGlEnable(const bool enable, const GLenum type)
{
	if (enable)
		glEnable(type);
	else
		glDisable(type);
}

void ModuleRenderExercise::RenderToTexture() {

	int w = 0;
	int h = 0;
	SDL_GetWindowSize(App->window->window, &w, &h);

	//buffer to texture

	GLuint FramebufferName = 0;
	glGenFramebuffers(1, &FramebufferName);
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

	glDeleteTextures(1, &renderedTexture);
	glGenTextures(1, &renderedTexture);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, renderedTexture);

	// Give an empty image to OpenGL ( the last "0" )
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	// Poor filtering. Needed !
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);

	// Set "renderedTexture" as our colour attachement #0
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderedTexture, 0);

	glDeleteRenderbuffers(1, &depthrenderbuffer);
	glGenRenderbuffers(1, &depthrenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

	glViewport(0, 0, w, h);
	glClearColor(background.x, background.y, background.z, background.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Draw();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}
