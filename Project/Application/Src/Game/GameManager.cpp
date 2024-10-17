#include "GameManager.h"

void GameManager::Initialize()
{
	//基底クラスの初期化
	GameCore::Initialize();

	//シーンの生成
	sceneFactory_ = std::make_unique<SceneFactory>();
	sceneManager_->SetSceneFactory(sceneFactory_.get());
	sceneManager_->ChangeScene("SampleScene");

	//ゲームオブジェクトファクトリーを生成
	gameObjectFactory_ = std::make_unique<GameObjectFactory>();
	gameObjectManager_->SetGameObjectFactory(gameObjectFactory_.get());

	//衝突属性の追加
	collisionAttributeManager_->AddAttribute("Player",       0b0000001, 0b1111100);
	collisionAttributeManager_->AddAttribute("PlayerWeapon", 0b0000010, 0b0000100);
	collisionAttributeManager_->AddAttribute("Enemy",        0b0000100, 0b0000011);
	collisionAttributeManager_->AddAttribute("EnemyWeapon",  0b0001000, 0b0000001);
	collisionAttributeManager_->AddAttribute("Missile",      0b0010000, 0b0000011);
	collisionAttributeManager_->AddAttribute("Laser",        0b0100000, 0b0000001);
	collisionAttributeManager_->AddAttribute("Pillar",       0b1000000, 0b0000001);

	//ポストエフェクトの設定
	postEffects_->SetIsEnable(true);
	postEffects_->GetFog()->SetIsEnable(true);
	postEffects_->GetBloom()->SetIsEnable(true);
	postEffects_->GetBloom()->SetBlurCount(4);
	postEffects_->GetBloom()->SetTextureWeight(1.0f);
	postEffects_->GetBloom()->SetHighLumTextureWeight(0.1f);
	postEffects_->GetBloom()->SetBlurTextureWeight(0, 0.1f);
	postEffects_->GetBloom()->SetBlurTextureWeight(1, 0.1f);
	postEffects_->GetBloom()->SetBlurTextureWeight(2, 0.2f);
	postEffects_->GetBloom()->SetBlurTextureWeight(3, 0.4f);
}