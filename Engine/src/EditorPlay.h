#pragma once
#include "EditorComponent.h"
#include "Time.h"

class EditorPlay : public EditorComponent
{
public:
    EditorPlay() {type = Play;};
    ~EditorPlay() {};

    void Draw(int w, int h);
};
