#include "PlayerStateGroundAttack.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Weapon/Weapon.h"
#include "PlayerStateIdle.h"

//コンボ定数表
const std::array<PlayerStateGroundAttack::ConstGroundAttack, PlayerStateGroundAttack::kComboNum> PlayerStateGroundAttack::kConstAttacks_ =
{
	{
		{ 10, 10, 10, 10, { 0.0f,0.01f,0.0f }, { 0.0f,0.01f,0.0f }, { 0.0f,0.01f,0.0f } },
        { 10, 10, 10, 10, { 0.01f,0.0f,0.0f }, { 0.01f,0.0f,0.0f }, { 0.01f,0.0f,0.0f } },
		{ 10, 10, 10, 10, { 0.0f,0.01f,0.0f }, { 0.0f,0.01f,0.0f }, { 0.0f,0.01f,0.0f } },
		{ 10, 10, 10, 10, { 0.01f,0.0f,0.0f }, { 0.01f,0.0f,0.0f }, { 0.01f,0.0f,0.0f } },
	}
};

void PlayerStateGroundAttack::Initialize()
{
	//Inputのインスタンスを取得
	input_ = Input::GetInstance();

	//環境変数の設定
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";
	globalVariables->CreateGroup(groupName);
}

void PlayerStateGroundAttack::Update()
{
	//コンボ上限に達していない
	if (workAttack_.comboIndex < kComboNum - 1)
	{
		//攻撃ボタンをトリガーしたら
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_X))
		{
			//コンボ有効
			workAttack_.comboNext = true;
		}
	}

	//コンボの合計時間
	uint32_t anticipationTime = kConstAttacks_[workAttack_.comboIndex].anticipationTime;
	uint32_t chargeTime = anticipationTime + kConstAttacks_[workAttack_.comboIndex].chargeTime;
	uint32_t swingTime = chargeTime + kConstAttacks_[workAttack_.comboIndex].swingTime;
	uint32_t recoveryTime = swingTime + kConstAttacks_[workAttack_.comboIndex].recoveryTime;

	//基底の時間経過で通常状態にも戻る
	if (++workAttack_.attackParameter > recoveryTime)
	{
		//コンボ継続なら次のコンボに進む
		if (workAttack_.comboNext)
		{
			//コンボ用パラメータをリセット
			workAttack_.comboNext = false;
			workAttack_.comboIndex++;
			workAttack_.attackParameter = 0;

			//コンボ切り替わりの瞬間だけ、スティック入力による方向転換を受け受ける
			const float threshold = 0.7f;

			//回転方向
			Vector3 direction = {
				input_->GetLeftStickX(),
				0.0f,
				input_->GetLeftStickY(),
			};

			//スティックの入力が遊び範囲を超えていたら回転フラグをtrueにする
			if (Mathf::Length(direction) > threshold)
			{
				//回転角度を正規化
				direction = Mathf::Normalize(direction);

				//移動ベクトルをカメラの角度だけ回転する
				Matrix4x4 rotateMatrix = Mathf::MakeRotateYMatrix(player_->camera_->rotation_.y);
				direction = Mathf::TransformNormal(direction, rotateMatrix);

				//回転処理
				Vector3 cross = Mathf::Normalize(Mathf::Cross({ 0.0f,0.0f,1.0f }, direction));
				float dot = Mathf::Dot({ 0.0f,0.0f,1.0f }, direction);
				player_->destinationQuaternion_ = Mathf::MakeRotateAxisAngleQuaternion(cross, std::acos(dot));
			}

			//次のコンボ用に回転角を初期化
			Weapon* weapon = GameObjectManager::GetInstance()->GetGameObject<Weapon>();
			TransformComponent* transformComponent = weapon->GetComponent<TransformComponent>();
			switch (workAttack_.comboIndex)
			{
			case 0:
				transformComponent->worldTransform_.rotation_ = { 0.0f,0.0f,0.0f };
				break;												 
			case 1:													 
				transformComponent->worldTransform_.rotation_ = { 0.0f,0.0f,0.0f };
				break;												   
			case 2:													   
				transformComponent->worldTransform_.rotation_ = { 0.0f,0.0f,0.0f };
				break;												  
			case 3:													  
				transformComponent->worldTransform_.rotation_ = { 0.0f,0.0f,0.0f };
				break;
			}
		}
		//コンボ継続でなければ通常状態に戻す
		else
		{
			player_->ChangeState(new PlayerStateIdle());
		}
	}

	//アニメーション処理
	if (workAttack_.comboIndex < kComboNum)
	{
		//武器のTransformを取得
		Weapon* weapon = GameObjectManager::GetInstance()->GetGameObject<Weapon>();
		TransformComponent* transformComponent = weapon->GetComponent<TransformComponent>();

		//振りかぶりアニメーション
		if (workAttack_.attackParameter >= 0.0f && workAttack_.attackParameter < anticipationTime)
		{
			transformComponent->worldTransform_.rotation_ += kConstAttacks_[workAttack_.comboIndex].anticipationSpeed;
		}
		//チャージアニメーション
		else if (workAttack_.attackParameter >= anticipationTime && workAttack_.attackParameter < chargeTime)
		{
			transformComponent->worldTransform_.rotation_ += kConstAttacks_[workAttack_.comboIndex].chargeSpeed;
		}
		//攻撃振りアニメーション
		else if (workAttack_.attackParameter >= chargeTime && workAttack_.attackParameter < swingTime)
		{
			transformComponent->worldTransform_.rotation_ += kConstAttacks_[workAttack_.comboIndex].swingSpeed;
		}

	}
}

void PlayerStateGroundAttack::Draw(const Camera& camera)
{
}

void PlayerStateGroundAttack::ApplyGlobalVariables()
{
}