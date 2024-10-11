#pragma once

class Pillar;

/// <summary>
/// 柱の状態
/// </summary>
class IPillarState
{
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~IPillarState() = default;

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// 柱を設定
	/// </summary>
	/// <param name="pillar">柱</param>
	void SetPillar(Pillar* pillar) { pillar_ = pillar; };

protected:
	//柱
	Pillar* pillar_ = nullptr;
};

