/**
 * @file PSO.cpp
 * @brief PSOの基底クラスを管理するファイル
 * @author 青木智滉
 * @date
 */

#include "PSO.h"

void PSO::SetRootSignature(const RootSignature* rootSignature)
{
	rootSignature_ = rootSignature;
}