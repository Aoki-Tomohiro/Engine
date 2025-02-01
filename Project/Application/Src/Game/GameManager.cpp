/**
 * @file GameManager.cpp
 * @brief ゲーム全体を管理するファイル
 * @author 青木智滉
 * @date
 */

#include "GameManager.h"

void GameManager::Initialize()
{
	//基底クラスの初期化
	GameCore::Initialize();

	//シーンの生成
	sceneFactory_ = std::make_unique<SceneFactory>();
	sceneManager_->SetSceneFactory(sceneFactory_.get());
	sceneManager_->ChangeScene("GameTitleScene");

	//ゲームオブジェクトファクトリーを生成
	gameObjectFactory_ = std::make_unique<GameObjectFactory>();
	gameObjectManager_->SetGameObjectFactory(gameObjectFactory_.get());

	//衝突属性の追加
	collisionAttributeManager_->AddAttribute("Player",          0b0000001, 0b0011100);
	collisionAttributeManager_->AddAttribute("PlayerWeapon",    0b0000010, 0b0010100);
	collisionAttributeManager_->AddAttribute("Enemy",           0b0000100, 0b0010011);
	collisionAttributeManager_->AddAttribute("EnemyWeapon",     0b0001000, 0b0010001);
	collisionAttributeManager_->AddAttribute("BreakableObject", 0b0010000, 0b0001111);

	//ポストエフェクトの設定
	postEffects_->SetIsEnable(true);

	//DoFの設定
	DepthOfField* depthOfField = postEffects_->GetDepthOfField();
	depthOfField->SetFocusDepth(9.6f);
	depthOfField->SetNFocusWidth(1.0f);
	depthOfField->SetFFocusWidth(10.0f);

	//Fogの設定
	Fog* fog = postEffects_->GetFog();
	fog->SetIsEnable(true);

	//Bloomの設定
	Bloom* bloom = postEffects_->GetBloom();
	bloom->SetIsEnable(true);
	bloom->SetBlurCount(4);
	bloom->SetTextureWeight(1.0f);
	bloom->SetHighLumTextureWeight(0.1f);
	bloom->SetBlurTextureWeight(0, 0.1f);
	bloom->SetBlurTextureWeight(1, 0.1f);
	bloom->SetBlurTextureWeight(2, 0.2f);
	bloom->SetBlurTextureWeight(3, 0.4f);
}