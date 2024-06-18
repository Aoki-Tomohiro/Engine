#pragma once
#include "Component.h"
#include "Engine/3D/Camera/Camera.h"

class RenderComponent : public Component
{
public:
	virtual void Draw(const Camera& camera) = 0;
};

