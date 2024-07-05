#include "GameObject.h"

void GameObject::Initialize()
{
}

void GameObject::Update()
{
	for (const std::unique_ptr<Component>& component : components_)
	{
		component->Update();
	}
}

void GameObject::Draw(const Camera& camera)
{
	for (const auto& component : components_)
	{
		if (auto* renderComponent = dynamic_cast<RenderComponent*>(component.get()))
		{
			renderComponent->Draw(camera);
		}
	}
}

void GameObject::DrawUI()
{

}