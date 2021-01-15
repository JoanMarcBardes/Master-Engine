#pragma once
#include "EditorComponent.h"

class EditorViewport : public EditorComponent
{
public:
    EditorViewport() { type = Viewport; };
    ~EditorViewport() {};

    void Draw(int w, int h);
}; 
