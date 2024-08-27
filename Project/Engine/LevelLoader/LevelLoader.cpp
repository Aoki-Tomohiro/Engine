#include "LevelLoader.h"
#include "Engine/3D/Model/ModelManager.h"
#include "Engine/3D/Camera/CameraManager.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Engine/Components/Model/ModelComponent.h"
#include "Engine/Components/Transform/TransformComponent.h"
#include "Engine/Components/Collision/AABBCollider.h"
#include "Engine/Components/Collision/OBBCollider.h"
#include "Engine/Components/Collision/SphereCollider.h"
#include "Engine/Components/Collision/CollisionAttributeManager.h"
#include <numbers>

const std::string LevelLoader::kBaseDirectory = "Application/Resources/LevelData/";

LevelLoader* LevelLoader::GetInstance()
{
	static LevelLoader instance;
	return &instance;
}

void LevelLoader::Load(const std::string& fileName)
{
	LevelLoader::GetInstance()->LoadInternal(fileName);
}

void LevelLoader::LoadInternal(const std::string& fileName)
{
	//読み込んだことのあるレベルデータなら再作成
	auto it = levelDatas_.find(fileName);
	if (it != levelDatas_.end())
	{
		CreateGameObjects(it->second.get());
		return;
	}

	//連結してフルパスを得る
	const std::string fullpath = kBaseDirectory + fileName + ".json";

	//ファイルストリーム
	std::ifstream file;

	//ファイルを開く
	file.open(fullpath);
	//ファイルオープン失敗をチェック
	if (file.fail())
	{
		assert(0);
	}

	//JSON文字列から連結したデータ
	nlohmann::json deserialized;

	//解凍
	file >> deserialized;

	//正しいレベルデータファイルかチェック
	assert(deserialized.is_object());
	assert(deserialized.contains("name"));
	assert(deserialized["name"].is_string());

	//"name"を文字列として取得
	std::string name = deserialized["name"].get<std::string>();
	//正しいレベルデータファイルかチェック
	assert(name.compare("scene") == 0);

	//レベルデータ格納用のインスタンスを生成
	LevelData* levelData = new LevelData();
	//"objects"の全オブジェクトを走査
	for (nlohmann::json& object : deserialized["objects"])
	{
		ProcessObject(object, levelData);
	}

	//読み込んだゲームオブジェクトを作成
	CreateGameObjects(levelData);

	//レベルデータをマップに保存
	levelDatas_[fileName] = std::unique_ptr<LevelData>(levelData);
}

void LevelLoader::ProcessObject(const nlohmann::json& object, LevelData* levelData)
{
	assert(object.contains("type"));
	//種別を取得
	std::string type = object["type"].get<std::string>();

	//Meshの場合
	if (type.compare("MESH") == 0)
	{
		//要素を追加
		levelData->objects.emplace_back(ObjectData{});

		//今回追加した要素の参照を得る
		ObjectData& objectData = levelData->objects.back();

		//オブジェクトの名前を取得
		if (object.contains("name"))
		{
			objectData.objectName = object["name"];
		}

		//モデルの名前を取得
		if (object.contains("file_name"))
		{
			objectData.modelName = object["file_name"];
		}

		//Activeフラグを取得
		if (object.contains("visible"))
		{
			objectData.isVisible = object["visible"];
		}

		//トランスフォーム
		nlohmann::json transform = object["transform"];
		//平行移動
		objectData.translation = { (float)transform["translation"][0] ,(float)transform["translation"][2] ,(float)transform["translation"][1] };
		//回転角
		objectData.rotation = { (float)transform["rotation"][0] ,(float)transform["rotation"][2] ,(float)transform["rotation"][1] };
		//スケーリング
		objectData.scaling = { (float)transform["scaling"][0] ,(float)transform["scaling"][2] ,(float)transform["scaling"][1] };

		//Collider
		if (object.contains("collider"))
		{
			nlohmann::json collider = object["collider"];
			objectData.colliderData.type = collider["type"].get<std::string>();
			objectData.colliderData.attribute = collider["attribute"].get<std::string>();
			if (objectData.colliderData.type == "AABB")
			{
				objectData.colliderData.center = { (float)collider["center"][0],(float)collider["center"][2] ,(float)collider["center"][1] };
				objectData.colliderData.size = { (float)collider["size"][0],(float)collider["size"][2] ,(float)collider["size"][1] };
			}
			else if (objectData.colliderData.type == "OBB")
			{
				objectData.colliderData.center = { (float)collider["center"][0],(float)collider["center"][2] ,(float)collider["center"][1] };
				objectData.colliderData.size = { (float)collider["size"][0],(float)collider["size"][2] ,(float)collider["size"][1] };
			}
			else if (objectData.colliderData.type == "SPHERE")
			{
				objectData.colliderData.center = { (float)collider["center"][0],(float)collider["center"][2] ,(float)collider["center"][1] };
				objectData.colliderData.radius = (float)collider["radius"];
			}
		}
	}
	else if (type.compare("CAMERA") == 0)
	{
		//要素を追加
		levelData->cameras.emplace_back(CameraData{});
		//今回追加した要素の参照を得る
		CameraData& cameraData = levelData->cameras.back();

		//カメラの名前を取得
		if (object.contains("name"))
		{
			cameraData.name = object["name"];
		}

		//トランスフォーム
		nlohmann::json transform = object["transform"];
		//平行移動
		cameraData.translation = { (float)transform["translation"][0] ,(float)transform["translation"][2] ,(float)transform["translation"][1] };
		//回転角
		cameraData.rotation = { std::numbers::pi_v<float> / 2.0f - (float)transform["rotation"][0] ,(float)transform["rotation"][2], (float)transform["rotation"][1] };
	}

	//オブジェクト捜査を再起関数にまとめ、再起呼び出しで枝を捜査する
	if (object.contains("children"))
	{
		for (const nlohmann::json& child : object["children"])
		{
			ProcessObject(child, levelData);
		}
	}
}

void LevelLoader::CreateGameObjects(const LevelData* levelData)
{
	//レベルデータからすべてのオブジェクトを生成
	for (auto& objectData : levelData->objects)
	{
		//3Dオブジェクトを生成
		GameObject* newObject = GameObjectManager::CreateGameObject(objectData.objectName);
		newObject->SetIsVisible(objectData.isVisible);

		//トランスフォームの追加
		TransformComponent* transformComponent = newObject->GetComponent<TransformComponent>();
		transformComponent->worldTransform_.translation_ = objectData.translation;
		transformComponent->worldTransform_.rotation_ = objectData.rotation;
		transformComponent->worldTransform_.quaternion_ = Mathf::Normalize(
			Mathf::MakeRotateAxisAngleQuaternion({ 0.0f,0.0f,1.0f }, objectData.rotation.z) *
			Mathf::MakeRotateAxisAngleQuaternion({ 0.0f,1.0f,0.0f }, objectData.rotation.y) *
			Mathf::MakeRotateAxisAngleQuaternion({ 1.0f,0.0f,0.0f }, objectData.rotation.x));
		transformComponent->worldTransform_.scale_ = objectData.scaling;

		//モデルの追加
		ModelComponent* modelComponent = newObject->AddComponent<ModelComponent>();
		modelComponent->SetModel(ModelManager::CreateFromModelFile(objectData.modelName, Opaque));

		//Typeが無かったらColliderがないとみなす
		if (objectData.colliderData.type != "")
		{
			CollisionAttributeManager* collisionAttributeManager = CollisionAttributeManager::GetInstance();
			if (objectData.colliderData.type == "AABB")
			{
				AABBCollider* collider = newObject->AddComponent<AABBCollider>();
				collider->SetCollisionAttribute(collisionAttributeManager->GetAttribute(objectData.colliderData.attribute));
				collider->SetCollisionMask(collisionAttributeManager->GetMask(objectData.colliderData.attribute));
				collider->SetCenter(objectData.colliderData.center);
				collider->SetMin({ -objectData.colliderData.size.x / 2.0f, -objectData.colliderData.size.y / 2.0f, -objectData.colliderData.size.z / 2.0f });
				collider->SetMax({ objectData.colliderData.size.x / 2.0f, objectData.colliderData.size.y / 2.0f, objectData.colliderData.size.z / 2.0f });
			}
			else if (objectData.colliderData.type == "OBB")
			{
				OBBCollider* collider = newObject->AddComponent<OBBCollider>();
				collider->SetCollisionAttribute(collisionAttributeManager->GetAttribute(objectData.colliderData.attribute));
				collider->SetCollisionMask(collisionAttributeManager->GetMask(objectData.colliderData.attribute));
				collider->SetCenter(objectData.colliderData.center);
				collider->SetSize(objectData.colliderData.size);
				collider->SetOrientations({ 1.0f,0.0f,0.0f }, { 0.0f,1.0f,0.0f }, { 0.0f,0.0f,1.0f });
			}
			else if (objectData.colliderData.type == "SPHERE")
			{
				SphereCollider* collider = newObject->AddComponent<SphereCollider>();
				collider->SetCollisionAttribute(collisionAttributeManager->GetAttribute(objectData.colliderData.attribute));
				collider->SetCollisionMask(collisionAttributeManager->GetMask(objectData.colliderData.attribute));
				collider->SetCenter(objectData.colliderData.center);
				collider->SetRadius(objectData.colliderData.radius);
			}
		}
	}

	//レベルデータからすべてのカメラを生成
	for (auto& cameraData : levelData->cameras)
	{
		//カメラの作成
		Camera* camera = CameraManager::CreateCamera(cameraData.name);
		camera->translation_ = cameraData.translation;
		camera->rotation_ = cameraData.rotation;
	}
}