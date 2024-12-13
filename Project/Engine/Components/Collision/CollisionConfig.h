/**
 * @file CollisionConfig.h
 * @brief 衝突属性の設定
 * @author 青木智滉
 * @date
 */

#pragma once
#include <cstdint>

//形状
const uint32_t kCollisionPrimitiveSphere = 0b1;
const uint32_t kCollisionPrimitiveAABB = kCollisionPrimitiveSphere << 1;
const uint32_t kCollisionPrimitiveOBB = kCollisionPrimitiveAABB << 1;