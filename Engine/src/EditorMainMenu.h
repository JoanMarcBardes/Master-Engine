#pragma once
#include "EditorComponent.h"
#include "ModuleEditor.h"

class EditorMainMenu : public EditorComponent
{
public:
<<<<<<< Updated upstream
    EditorMainMenu(ModuleEditor* modE) { type = MainMenu; 
    SetModule(modE);
    };
=======
    EditorMainMenu() { type = MainMenu;};
>>>>>>> Stashed changes
    ~EditorMainMenu() {};

    void Draw(int w, int h);

<<<<<<< Updated upstream
    void SetModule(ModuleEditor* modE) { mod = modE; };

private:
    ModuleEditor* mod;

    bool activeAbout = false;
    bool activeConsole = true;
    bool activeConfig = true;
    bool activeHierarchy = true;
    bool activeInspector = true;
    bool activePlay = true;
=======
private:
>>>>>>> Stashed changes

    void RequestBrowser(const char* url);
};