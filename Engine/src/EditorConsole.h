#pragma once
#include "EditorComponent.h"

class EditorConsole : public EditorComponent
{
public:
    EditorConsole() { type = Console; };
    ~EditorConsole() {};

    void Draw(int w, int h);


};
