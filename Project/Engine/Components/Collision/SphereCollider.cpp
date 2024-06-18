#include "SphereCollider.h"

void SphereCollider::Initialize()
{

}

void SphereCollider::Update()
{
	if (transformComponent_ != nullptr)
	{
		center_ = transformComponent_->translation_;
	}
}