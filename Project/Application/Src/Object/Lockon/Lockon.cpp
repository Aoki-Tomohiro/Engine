#include "Lockon.h"

void Lockon::Initialize()
{
	//インスタンスを取得
	input_ = Input::GetInstance();

	//スプライトの生成
	TextureManager::Load("Reticle.png");
	lockOnMark_.reset(Sprite::Create("Reticle.png", { 0.0f,0.0f }));
	lockOnMark_->SetAnchorPoint({ 0.5f,0.5f });
}

void Lockon::Update(const Camera* camera)
{
	//ロックオン状態なら
	if (target_)
	{
		//ロックオンマークの座標計算
		SetLockOnMarkPosition(camera);

		//ロックオン解除処理
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_LEFT_SHOULDER))
		{
			//ロックオンを外す
			target_ = nullptr;
		}
	}
	else
	{
		//ロックオン対象の検索
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_LEFT_SHOULDER))
		{
			target_ = GameObjectManager::GetInstance()->GetConstGameObject<Enemy>("");
			SetLockOnMarkPosition(camera);
		}
	}
}

void Lockon::Draw()
{
	if (target_)
	{
		lockOnMark_->Draw();
	}
}

const Vector3 Lockon::GetTargetPosition() const
{
	if (target_)
	{
		return target_->GetHipWorldPosition();
	}
	return Vector3();
}

Vector2 Lockon::WorldToScreenPosition(const Vector3& worldPosition, const Camera* camera)
{
	// ビューポート行列
	Matrix4x4 matViewport = Mathf::MakeViewportMatrix(0, 0, Application::kClientWidth, Application::kClientHeight, 0, 1);
	// ビュー行列とプロジェクション行列、ビューポート行列を合成する
	Matrix4x4 matViewProjectionViewport = camera->matView_ * camera->matProjection_ * matViewport;
	//ワールド座標からスクリーン座標に変換
	Vector3 positionScreen = Mathf::Transform(worldPosition, matViewProjectionViewport);
	//Vector2に変換して返す
	return Vector2(positionScreen.x, positionScreen.y);
}

void Lockon::SetLockOnMarkPosition(const Camera* camera)
{
	//敵のロックオン座標取得
	Vector2 positionScreenV2 = WorldToScreenPosition(target_->GetHipWorldPosition(), camera);
	lockOnMark_->SetPosition(positionScreenV2);
}