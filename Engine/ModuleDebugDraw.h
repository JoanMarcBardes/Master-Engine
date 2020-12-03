#pragma once
#include "Module.h"
#include "MathGeoLib/Math/float4x4.h"

class DDRenderInterfaceCoreGL;
class Camera;

class ModuleDebugDraw : public Module
{

public:

	bool            Init() override;
	bool            CleanUp() override;

    void            Draw(const float4x4& view, const float4x4& proj, unsigned width, unsigned height);
private:

    static DDRenderInterfaceCoreGL* implementation;
};
