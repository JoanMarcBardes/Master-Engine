#pragma once
#include "EditorComponent.h"

class EditorAbout : public EditorComponent
{
public:
    EditorAbout() { type = About; };
    ~EditorAbout() {};

    void Draw(int w, int h);
};

