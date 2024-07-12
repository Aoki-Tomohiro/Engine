#include "CollisionAttributeManager.h"

CollisionAttributeManager* CollisionAttributeManager::GetInstance()
{
	static CollisionAttributeManager instance;
	return &instance;
}

void CollisionAttributeManager::AddAttribute(const std::string& name, const uint32_t attribute, const uint32_t mask)
{
	if (attributes_.find(name) == attributes_.end())
	{
		attributes_[name] = attribute;
		masks_[name] = mask;
	}
}

const uint32_t CollisionAttributeManager::GetAttribute(const std::string& name) const
{
	auto it = attributes_.find(name);
	if (it != attributes_.end())
	{
		return it->second;
	}
	return 0xffffffff;
}

const uint32_t CollisionAttributeManager::GetMask(const std::string& name) const
{
	auto it = masks_.find(name);
	if (it != masks_.end())
	{
		return it->second;
	}
	return 0xffffffff;
}