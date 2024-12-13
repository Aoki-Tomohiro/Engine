/**
 * @file CollisionAttributeManager.h
 * @brief 衝突属性を管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>

class CollisionAttributeManager
{
public:
    /// <summary>
    /// インスタンスを取得
    /// </summary>
    /// <returns>インスタンス</returns>
    static CollisionAttributeManager* GetInstance();

    /// <summary>
    /// 衝突属性を追加
    /// </summary>
    /// <param name="name">名前</param>
    /// <param name="attribute">衝突属性</param>
    /// <param name="mask">衝突マスク</param>
    void AddAttribute(const std::string& name, const uint32_t attribute, const uint32_t mask);

    //衝突属性を取得
    const uint32_t GetAttribute(const std::string& name) const;

    //衝突マスクを取得
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

