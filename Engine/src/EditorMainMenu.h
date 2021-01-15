#pragma once
#include "EditorComponent.h"
#include "ModuleEditor.h"

class EditorMainMenu : public EditorComponent
{
public:
    EditorMainMenu() { type = MainMenu;};
    ~EditorMainMenu() {};

    void Draw(int w, int h);

private:

    void RequestBrowser(const char* url);
};