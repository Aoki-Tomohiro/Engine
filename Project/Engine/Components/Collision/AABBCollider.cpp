#include "AABBCollider.h"

void AABBCollider::Initialize()
{

}

void AABBCollider::Update()
{
	if (transformComponent_ != nullptr)
	{
		center_ = transformComponent_->translation_;
	}
}