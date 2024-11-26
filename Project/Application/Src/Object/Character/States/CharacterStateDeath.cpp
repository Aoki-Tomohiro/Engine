#include "CharacterStateDeath.h"
#include "Application/Src/Object/Character/BaseCharacter.h"

void CharacterStateDeath::Initialize()
{
	//アニメーションの再生
	SetAnimationControllerAndPlayAnimation("Death");
}

void CharacterStateDeath::Update()
{
	//ノックバックの処理
	character_->ApplyKnockback();

	//アニメーションイベントを実行
	UpdateAnimationState();

	//地面に埋まらないように座標を補正
	ClampPositionToGround();

	//カメラアニメーションイベントが終了したらタイムスケールをもとに戻す
	if (!processedCameraAnimationDatas_.empty() && !processedCameraAnimationDatas_[0].isActive)
	{
		//タイムスケールを元に戻す
		GameTimer::SetTimeScale(1.0f);
	}
	else
	{
		//タイムスケールを設定
		GameTimer::SetTimeScale(0.2f);
	}

	//アニメーションが終了しているかつ地面にいる場合
	if (character_->GetAnimator()->GetIsAnimationFinished() && character_->GetPosition().y == 0.0f)
	{
		//ゲーム終了フラグを立てる
		character_->SetIsGameFinished(true);
	}
}

void CharacterStateDeath::ClampPositionToGround()
{
	//現在のキャラクターの位置を取得
	Vector3 currentPosition = character_->GetPosition();

	//地面に到達した場合
	if (currentPosition.y < 0.0f)
	{
		//キャラクターの位置を地面に合わせる
		character_->SetPosition({ currentPosition.x, 0.0f, currentPosition.z });
	}
}