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
	Quaternion rotation = Mathf::MakeRotateAxisAngleQuaternion(Mathf::Normalize(Vector3{ 1.0f,0.4f,-0.2f }), 0.45f);
	Vector3 pointY = { 2.1f,-0.9f,1.3f };
	Matrix4x4 rotateMatrix = Mathf::MakeRotateMatrix(rotation);
	Vector3 rotateByQuaternion = Mathf::RotateVector(pointY, rotation);
	Vector3 rotateByMatrix = Mathf::Transform(pointY, rotateMatrix);

	ImGui::Begin("TitleScene");
	ImGui::Text("%1.2f %1.2f %1.2f %1.2f : rotation",
		rotation.x, rotation.y, rotation.z, rotation.w);
	ImGui::Text("rotateMatrix\n%1.3f %1.3f %1.3f %1.3f\n%1.3f %1.3f %1.3f %1.3f\n%1.3f %1.3f %1.3f %1.3f\n%1.3f %1.3f %1.3f %1.3f",
		rotateMatrix.m[0][0], rotateMatrix.m[0][1], rotateMatrix.m[0][2], rotateMatrix.m[0][3],
		rotateMatrix.m[1][0], rotateMatrix.m[1][1], rotateMatrix.m[1][2], rotateMatrix.m[1][3],
		rotateMatrix.m[2][0], rotateMatrix.m[2][1], rotateMatrix.m[2][2], rotateMatrix.m[2][3],
		rotateMatrix.m[3][0], rotateMatrix.m[3][1], rotateMatrix.m[3][2], rotateMatrix.m[3][3]);
	ImGui::Text("%1.2f %1.2f %1.2f : rotateByQuaternion",
		rotateByQuaternion.x, rotateByQuaternion.y, rotateByQuaternion.z);
	ImGui::Text("%1.2f %1.2f %1.2f : rotateByMatrix",
		rotateByMatrix.x, rotateByMatrix.y, rotateByMatrix.z);
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