#include "Lockon.h"
#include "Engine/Framework/Object/GameObjectManager.h"

void Lockon::Initialize()
{
	//インプットのインスタンスを取得
	input_ = Input::GetInstance();

	//スプライトの生成
	const std::string texturePath = "Reticle.png";
	TextureManager::Load(texturePath);
	lockonMark_.reset(Sprite::Create(texturePath, { 0.0f,0.0f }));
	lockonMark_->SetAnchorPoint({ 0.5f,0.5f });
}

void Lockon::Update(const Camera* camera)
{
	//ロックオンターゲットの更新
	UpdateTargeting();

	if (target_)
	{
		//ロックオン中の場合スプライトの座標を更新
		SetLockonMarkPosition(camera);
	}
}

void Lockon::Draw()
{
	//ターゲットがいる場合はロックオンマークを表示する
	if (target_)
	{
		lockonMark_->Draw();
	}
}

const Vector3 Lockon::GetTargetPosition() const
{
	//ターゲットがいる場合は腰のジョイントのワールド座標を返す
	return target_ ? target_->GetHipWorldPosition() : Vector3();
}

void Lockon::UpdateTargeting()
{
	//LBを押したとき
	if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_LEFT_SHOULDER))
	{
		//ターゲットがいる場合
		if (target_)
		{
			//ターゲットを解除
			target_ = nullptr;
		}
		else
		{
			//ターゲットを設定
			target_ = GameObjectManager::GetInstance()->GetMutableGameObject<Enemy>("");
		}
	}
}

Vector2 Lockon::WorldToScreenPosition(const Vector3& worldPosition, const Camera* camera)
{
	//ビューポート行列
	Matrix4x4 matViewport = Mathf::MakeViewportMatrix(0, 0, Application::kClientWidth, Application::kClientHeight, 0, 1);
	//ビュー行列とプロジェクション行列、ビューポート行列を合成する
	Matrix4x4 matViewProjectionViewport = camera->matView_ * camera->matProjection_ * matViewport;
	//ワールド座標からスクリーン座標に変換
	Vector3 positionScreen = Mathf::Transform(worldPosition, matViewProjectionViewport);
	//Vector2に変換して返す
	return Vector2(positionScreen.x, positionScreen.y);
}

void Lockon::SetLockonMarkPosition(const Camera* camera)
{
	//敵のロックオン座標取得
	Vector2 positionScreenV2 = WorldToScreenPosition(target_->GetHipWorldPosition(), camera);
	//スプライトの座標を設定
	lockonMark_->SetPosition(positionScreenV2);
}