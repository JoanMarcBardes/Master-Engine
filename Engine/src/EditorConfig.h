#pragma once
#include "EditorComponent.h"

class EditorConfig : public EditorComponent
{
public:
    EditorConfig() { type = Config;
    fpsLog.resize(fpsLogSize);
    msLog.resize(fpsLogSize);
    };
    ~EditorConfig() {
        fpsLog.clear();
        msLog.clear();
    };

    void Draw(int w, int h);

private:
    std::vector<int> fpsLog;
    std::vector<float> msLog;
    int fpsLogSize = 100;

};
