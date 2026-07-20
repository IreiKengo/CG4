#pragma once
#include "Vector3.h"
#include <vector>
#include <string>
#include <map>
#include "json.hpp"


class Object3dCommon;
class Object3d;

class LevelLoader
{

public:

	struct ColliderData {
		std::string type;
		Vector3 center;
		Vector3 size;
	};

	///レベルデータを格納するための構造体
//オブジェクト1個分のデータ
	struct ObjectData {
		std::string type;
		std::string name;

		struct Transform {
			Vector3 translation;
			Vector3 rotation;
			Vector3 scaling;
		};
		Transform transform;
		ColliderData collider;

		//file_name
		std::string fileName;

	};

	struct NodeObject {
		std::string name;
		std::vector<NodeObject> children; // リスト<Object> children
		// 自分が levelData->objects の何番目にいるかのメモ用（MESH以外なら-1）
		int objectDataIndex = -1;
	};

	//レベルデータ
	struct LevelData {
		//name
		std::string name;
		//objects
		std::vector<ObjectData> objects;

		// 生成した実体を保持するリスト
		std::vector<Object3d*> objectPtrs;
	};

	LevelData* LoadLevelJson(const std::string& fileName, Object3dCommon* object3dCommon);

	NodeObject ConvertJsonToObjects(const nlohmann::json& jsonNode, LevelData* levelData);

	void LinkObjectsParentRecursive(const NodeObject& node, LevelData* levelData, Object3d* currentParentPtr);


	static void DebugUpdate(LevelData* levelData);

};