#include "LevelLoader.h"
#include <fstream>
#include "json.hpp"
#include <cassert>
#include "Object3d.h"
//#include "Model.h"


LevelLoader::LevelData* LevelLoader::LoadLevelJson(const std::string& fileName, Object3dCommon* object3dCommon)
{

	//連続してフルパスを得る
	std::string kDefaultBaseDirectory = "Resources/levels/";

	const std::string fullpath = kDefaultBaseDirectory + fileName;

	//ファイルストリーム
	std::ifstream file;

	//ファイルを開く
	file.open(fullpath);
	//ファイルオープン失敗をチェック
	if (file.fail()) {
		assert(0 && "JSONファイルのオープンに失敗しました");
	}

	//JSON文字列から解凍したデータ
	nlohmann::json deserialized;

	//解凍
	file >> deserialized;

	//正しいレベルデータファイルかチェック
	assert(deserialized.is_object());
	assert(deserialized.contains("name"));
	assert(deserialized["name"].is_string());

	//"name"を文字列として取得
	std::string name = deserialized["name"].get<std::string>();
	// 正しいレベルデータファイルかチェック
	assert(name.compare("scene") == 0);

	//レベルデータ格納用インスタンスを生成
	LevelData* levelData = new LevelData();
	levelData->name = name;

	// "objects"の全オブジェクトを走査
	for (nlohmann::json& object : deserialized["objects"]) {
		assert(object.contains("type"));

		//種別を取得
		std::string type = object["type"].get<std::string>();

		//MESH
		if (type.compare("MESH") == 0) {
			// 要素追加
			levelData->objects.emplace_back(ObjectData{});
			// 今追加した要素の参照を得る
			ObjectData& objectData = levelData->objects.back();

			if (object.contains("file_name")) {
				//ファイル名
				objectData.fileName = object["file_name"];
			}
			//トランスフォームのパラメータ読み込み
			nlohmann::json& transform = object["transform"];
			//平行移動
			objectData.transform.translation.x = (float)transform["translation"][0];
			objectData.transform.translation.y = (float)transform["translation"][2];
			objectData.transform.translation.z = (float)transform["translation"][1];
			//回転角
			objectData.transform.rotation.x = -(float)transform["rotation"][0];
			objectData.transform.rotation.y = -(float)transform["rotation"][2];
			objectData.transform.rotation.z = -(float)transform["rotation"][1];
			//スケーリング
			objectData.transform.scaling.x = (float)transform["scaling"][0];
			objectData.transform.scaling.y = (float)transform["scaling"][2];
			objectData.transform.scaling.z = (float)transform["scaling"][1];

			// TODO:　コライダーのパラメータ読み込み

		}

		//再帰処理
		//TODO:　オブジェクト走査を再帰関数にまとめ、再帰呼出で枝を走査する
		if (object.contains("children")) {

		}

	}



	//レベルデータからオブジェクトを配生成、配置
	for (auto& objectData : levelData->objects) {
		
		//モデルを指定して3Dオブジェクトを生成
		Object3d* newObject = new Object3d();

		newObject->Initialize(object3dCommon);
		newObject->SetModel(objectData.fileName);
		//座標
		newObject->SetTranslate(objectData.transform.translation);
		//回転角
		newObject->SetRotate(objectData.transform.rotation);
		//スケール
		newObject->SetScale(objectData.transform.scaling);
		newObject->SetIsUseEnvironmentMap(false);

		//配列に登録
		levelData->objectPtrs.push_back(newObject);
	}
	return levelData;
}

