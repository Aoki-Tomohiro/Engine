#pragma once
class GameObject;

class Component
{
public:
	virtual ~Component() = default;

	virtual void Initialize() = 0;

	virtual void Update() = 0;

	GameObject* owner_ = nullptr;
};

