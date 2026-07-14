#pragma once
#include "Vector3.h"
#include <vector>
#include <string>
#include <map>



class Object3dCommon;
class Object3d;

class LevelLoader
{

public:
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

		//file_name
		std::string fileName;

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

};