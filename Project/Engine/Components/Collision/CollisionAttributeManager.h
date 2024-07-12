#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>

class CollisionAttributeManager
{
public:
    static CollisionAttributeManager* GetInstance();

    void AddAttribute(const std::string& name, const uint32_t attribute, const uint32_t mask);

    const uint32_t GetAttribute(const std::string& name) const;

    const uint32_t GetMask(const std::string& name) const;

private:
    CollisionAttributeManager() = default;
    ~CollisionAttributeManager() = default;
    CollisionAttributeManager(const CollisionAttributeManager&) = delete;
    const CollisionAttributeManager& operator = (const CollisionAttributeManager&) = delete;

private:
    std::unordered_map<std::string, uint32_t> attributes_;

    std::unordered_map<std::string, uint32_t> masks_;
};

