/**
 * @file Component.h
 * @brief コンポーネントの基底クラスを管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
class GameObject;

class Component
{
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~Component() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize() = 0;

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() = 0;

	//このコンポーネントを持っているゲームオブジェクト
	GameObject* owner_ = nullptr;
};

