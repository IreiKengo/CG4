#include "LevelLoader.h"
#include <fstream>
#include "json.hpp"
#include <cassert>
#include "Object3d.h"
#include "ImGuiManager.h"


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

	// 最上位のノードたちを一時的に保持するリスト
	std::vector<NodeObject> rootNodes;

	// "objects"の全オブジェクトを走査
	for (nlohmann::json& object : deserialized["objects"]) {
		assert(object.contains("type"));
		rootNodes.push_back(ConvertJsonToObjects(object, levelData));
		
		//再帰処理
		//TODO:　オブジェクト走査を再帰関数にまとめ、再帰呼出で枝を走査する
		//ConvertJsonToObjects(object,levelData);

		
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
		//環境マップ
		newObject->SetIsUseEnvironmentMap(false);

		//配列に登録
		levelData->objectPtrs.push_back(newObject);
	}

	for (const auto& rootNode : rootNodes) {
		LinkObjectsParentRecursive(rootNode, levelData, nullptr);
	}

	return levelData;
}

LevelLoader::NodeObject LevelLoader::ConvertJsonToObjects(const nlohmann::json& jsonNode,LevelLoader::LevelData* levelData)
{
	//JSONノードの情報を元にObjectを作成
	NodeObject object;
	object.name = jsonNode["name"].get<std::string>();
	object.objectDataIndex = -1; // 初期値は-1（MESHではないライトやカメラ用）

	assert(jsonNode.contains("type"));

	//種別を取得
	std::string type = jsonNode["type"].get<std::string>();

	//MESH
	if (type.compare("MESH") == 0) {
		// 要素追加
		levelData->objects.emplace_back(ObjectData{});
		// 今追加した要素の参照を得る
		ObjectData& objectData = levelData->objects.back();
		object.objectDataIndex = (int)levelData->objects.size() - 1;

		objectData.name = jsonNode["name"].get<std::string>();

		if (jsonNode.contains("file_name")) {
			//ファイル名
			objectData.fileName = jsonNode["file_name"].get<std::string>();
		}
		//トランスフォームのパラメータ読み込み
		const auto& transform = jsonNode["transform"];
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
		if (jsonNode.contains("collider"))
		{

			const auto& colliderJson = jsonNode["collider"];

			// 1つずつバラして型を指定して読み込む
			objectData.collider.type = colliderJson["type"].get<std::string>();

			
			objectData.collider.center.x = (float)colliderJson["center"][0];
			objectData.collider.center.y = (float)colliderJson["center"][2];
			objectData.collider.center.z = (float)colliderJson["center"][1];

			objectData.collider.size.x = (float)colliderJson["size"][0];
			objectData.collider.size.y = (float)colliderJson["size"][1];
			objectData.collider.size.z = (float)colliderJson["size"][2];

		}

	}

	//子ノードが存在する場合、再帰的に処理
	if (jsonNode.contains("children"))
	{
		for (const auto& childrens : jsonNode["children"])
		{
			NodeObject childObject = ConvertJsonToObjects(childrens,levelData);

			object.children.push_back(childObject);

		}


	}
	return object;
}


void LevelLoader::LinkObjectsParentRecursive(const NodeObject& node, LevelData* levelData, Object3d* currentParentPtr)
{
	// 今のノードが本物のオブジェクト（MESHなど）を指しているか確認
	Object3d* nextParentPtr = currentParentPtr;

	if (node.objectDataIndex != -1) {
		// インデックスを元に、生成済みの実体ポインタを取得
		Object3d* myObjectPtr = levelData->objectPtrs[node.objectDataIndex];

		// 親ポインタが存在するなら、親子関係をバインドする
		if (currentParentPtr != nullptr) {
			myObjectPtr->SetParent(currentParentPtr);
		}

		// 自分が、次の子供たちにとっての親になる
		nextParentPtr = myObjectPtr;
	}

	// 子供たちの階層へ再帰的に潜る
	for (const auto& childNode : node.children) {
		LinkObjectsParentRecursive(childNode, levelData, nextParentPtr);
	}
}

void LevelLoader::DebugUpdate(LevelData* levelData)
{

#ifdef USE_IMGUI
	

	ImGui::Begin("Level Editor Debug");

	
	// レベル内のオブジェクトを1つずつ処理
	for (size_t i = 0; i < levelData->objects.size(); ++i) {
		Object3d* objPtr = levelData->objectPtrs[i];
		if (!objPtr) continue;

		// --- 【ここから対策】 ---
		// 元々の名前を取得（もし空なら "NoName" にする）
		std::string originalName = levelData->objects[i].name;
		if (originalName.empty()) {
			originalName = "NoName";
		}

		// 名前が同じ、または空でもImGuiがバグらないように、
		// 「名前##ループインデックス」という文字列を作る
		// 例: "Cube##0", "Cube##1" (##以降はImGuiの画面には表示されず、内部IDとしてだけ使われます)
		std::string displayName = originalName + "##" + std::to_string(i);
		// --- 【ここまで対策】 ---

		if (ImGui::TreeNode(displayName.c_str())) {

			// Object3dから現在のトランスフォームを取得して書き換える
			Vector3 translate = objPtr->GetTranslate();
			Vector3 rotate = objPtr->GetRotate();
			Vector3 scale = objPtr->GetScale();

			if (ImGui::DragFloat3("Translate", &translate.x, 0.1f)) {
				objPtr->SetTranslate(translate);
			}
			if (ImGui::SliderFloat3("Rotate", &rotate.x, -3.14f, 3.14f)) {
				objPtr->SetRotate(rotate);
			}
			if (ImGui::DragFloat3("Scale", &scale.x, 0.1f)) {
				objPtr->SetScale(scale);
			}

			// Object3d自身のデバッグ表示（ライトや環境マップなど）
			objPtr->DebugUpdate();

			ImGui::TreePop(); // ツリーを閉じる
			ImGui::Separator();
		}
	}

	ImGui::End();

#endif 


}
