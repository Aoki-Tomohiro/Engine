#include "GameTitleScene.h"

void GameTitleScene::Initialize()
{
	renderer_ = Renderer::GetInstance();

	input_ = Input::GetInstance();

	audio_ = Audio::GetInstance();
}

void GameTitleScene::Finalize()
{

}

void GameTitleScene::Update() 
{
	Quaternion q1 = { 2.0f,3.0f,4.0f,1.0f };
	Quaternion q2 = { 1.0f,3.0f,5.0f,2.0f };
	Quaternion identity = Mathf::IdentityQuaternion();
	Quaternion conj = Mathf::Conjugate(q1);
	Quaternion inv = Mathf::Inverse(q1);
	Quaternion normal = Mathf::Normalize(q1);
	Quaternion mul1 = q1 * q2;
	Quaternion mul2 = q2 * q1;
	float norm = Mathf::Norm(q1);

	ImGui::Begin("TitleScene");
	ImGui::Text("%1.2f %1.2f %1.2f %1.2f     : Identity",
		identity.x, identity.y, identity.z, identity.w);
	ImGui::Text("%1.2f %1.2f %1.2f %1.2f  : Conjugate",
		conj.x, conj.y, conj.z, conj.w);
	ImGui::Text("%1.2f %1.2f %1.2f %1.2f  : Inverse",
		inv.x, inv.y, inv.z, inv.w);
	ImGui::Text("%1.2f %1.2f %1.2f %1.2f     : Normalize",
		normal.x, normal.y, normal.z, normal.w);
	ImGui::Text("%1.2f %1.2f %1.2f %1.2f  : Multiply(q1,q2)",
		mul1.x, mul1.y, mul1.z, mul1.w);
	ImGui::Text("%1.2f %1.2f %1.2f %1.2f : Multiply(q2,q1)",
		mul2.x, mul2.y, mul2.z, mul2.w);
	ImGui::Text("%1.2f                    : Norm", norm);
	ImGui::End();
}

void GameTitleScene::Draw()
{
#pragma region 背景スプライト描画
	//背景スプライト描画前処理
	renderer_->PreDrawSprites(kBlendModeNormal);

	//背景スプライト描画後処理
	renderer_->PostDrawSprites();
#pragma endregion

	//深度バッファをクリア
	renderer_->ClearDepthBuffer();

#pragma region 3Dオブジェクト描画
	//3Dオブジェクト描画
	renderer_->Render();
#pragma endregion

#pragma region パーティクル描画
	//パーティクル描画前処理
	renderer_->PreDrawParticles();

	//パーティクル描画後処理
	renderer_->PostDrawParticles();
#pragma endregion
}

void GameTitleScene::DrawUI() 
{
#pragma region 前景スプライト描画
	//前景スプライト描画前処理
	renderer_->PreDrawSprites(kBlendModeNormal);

	//前景スプライト描画後処理
	renderer_->PostDrawSprites();
#pragma endregion
}