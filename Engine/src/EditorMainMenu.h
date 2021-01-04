#pragma once
#include "EditorComponent.h"
#include "ModuleEditor.h"

class EditorMainMenu : public EditorComponent
{
public:
    EditorMainMenu(ModuleEditor* modE) { type = MainMenu; 
    SetModule(modE);
    };
    ~EditorMainMenu() {};

    void Draw(int w, int h);

    void SetModule(ModuleEditor* modE) { mod = modE; };

private:
    ModuleEditor* mod;

    bool activeAbout = false;
    bool activeConsole = true;
    bool activeConfig = true;
    bool activeHierarchy = true;
    bool activeInspector = true;
    bool activePlay = true;

    void RequestBrowser(const char* url);
};