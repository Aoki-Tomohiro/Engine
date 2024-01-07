#pragma once
#include <cstdint>

//CollisionAttribute
const uint32_t kCollisionAttributePlayer = 0b0001;
const uint32_t kCollisionAttributeEnemy = 0b0010;
const uint32_t kCollisionAttributeWeapon = 0b0100;

//CollisionMask
const uint32_t kCollisionMaskPlayer = 0b1010;
const uint32_t kCollisionMaskEnemy = 0b1101;
const uint32_t kCollisionMaskWeapon = 0b0010;

//形状
const uint32_t kCollisionPrimitiveSphere = 0b1;
const uint32_t kCollisionPrimitiveAABB = 0b1 << 1;
const uint32_t kCollisionPrimitiveOBB = kCollisionPrimitiveAABB << 1;