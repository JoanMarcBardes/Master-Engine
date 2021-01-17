#pragma once
#include "EditorComponent.h"
#include "GameObject.h"
#include "Camera.h"
#include "Transform.h"

class EditorViewport : public EditorComponent
{
public:
    EditorViewport() { type = Viewport; };
    ~EditorViewport() {};

    void Draw(int w, int h);
    void DrawGuizmo(Camera* camera, GameObject* go);
}; 
