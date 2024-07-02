#include "PlayerStateGroundAttack.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Weapon/Weapon.h"
#include "Application/Src/Object/Enemy/Enemy.h"
#include "PlayerStateIdle.h"

//コンボ定数表
const std::array<PlayerStateGroundAttack::ConstGroundAttack, PlayerStateGroundAttack::kComboNum> PlayerStateGroundAttack::kConstAttacks_ =
{
	{
		{ 10, 10, 10, 10, { 0.0f,0.06f,0.0f }, { 0.0f,0.06f,0.0f }, { 0.0f,0.06f,0.0f } },
        { 10, 10, 10, 10, { 0.06f,0.0f,0.0f }, { 0.06f,0.0f,0.0f }, { 0.06f,0.0f,0.0f } },
		{ 10, 10, 10, 10, { 0.0f,0.06f,0.0f }, { 0.0f,0.06f,0.0f }, { 0.0f,0.06f,0.0f } },
		{ 10, 10, 10, 10, { 0.06f,0.0f,0.0f }, { 0.06f,0.0f,0.0f }, { 0.06f,0.0f,0.0f } },
	}
};

void PlayerStateGroundAttack::Initialize()
{
	//Inputのインスタンスを取得
	input_ = Input::GetInstance();

	//武器を描画させる
	Weapon* weapon = GameObjectManager::GetInstance()->GetGameObject<Weapon>();
	weapon->SetIsVisible(true);

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
		else
		{
			//武器をリセット
			Weapon* weapon = GameObjectManager::GetInstance()->GetGameObject<Weapon>();
			TransformComponent* transformComponent = weapon->GetComponent<TransformComponent>();
			transformComponent->worldTransform_.rotation_ = { 0.0f,0.0f,0.0f };
			weapon->SetIsVisible(false);

			//通常状態に戻す
			player_->ChangeState(new PlayerStateIdle());
			return;
		}
	}

	//敵の座標を取得
	Enemy* enemy = GameObjectManager::GetInstance()->GetGameObject<Enemy>();
	TransformComponent* enemyTransformConponent = enemy->GetComponent<TransformComponent>();

	//差分ベクトルを計算
	TransformComponent* playerTransformConponent = player_->GetComponent<TransformComponent>();
	Vector3 sub = enemyTransformConponent->GetWorldPosition() - playerTransformConponent->GetWorldPosition();
	sub.y = 0.0f;

	//距離を計算
	float distance = Mathf::Length(sub);

	//閾値
	float threshold = 10.0f;

	//ボスとの距離が閾値より小さかったらボスの方向に回転させる
	if (distance < threshold || player_->lockOn_->ExistTarget())
	{
		//回転
		Vector3 cross = Mathf::Normalize(Mathf::Cross({ 0.0f,0.0f,1.0f }, Mathf::Normalize(sub)));
		float dot = Mathf::Dot({ 0.0f,0.0f,1.0f }, Mathf::Normalize(sub));
		player_->destinationQuaternion_ = Mathf::MakeRotateAxisAngleQuaternion(cross, std::acos(dot));
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

	//環境変数の適用
	ApplyGlobalVariables();
}

void PlayerStateGroundAttack::Draw(const Camera& camera)
{
}

void PlayerStateGroundAttack::ApplyGlobalVariables()
{
}