#pragma once
#include<string>
#include <wrl.h>
#include <d3d12.h>
#include "Transform.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4Math.h"


class Object3dCommon;
class DirectXCommon;
class Model;
class Camera;

//3Dオブジェクト
class Object3d
{

public:




	//座標変換行列データ
	struct TransformationMatrix
	{
		Matrix4x4 WVP;
		Matrix4x4 World;
		Matrix4x4 WorldInverseTranspose;
	};

	struct DirectionalLight
	{
		Vector4 color; //ライトの色
		Vector3 direction; //ライトの向き
		float intensity; //輝度
	};

	struct CameraForGPU
	{
		Vector3 worldPosition;
	};


	//点光源
	struct PointLight
	{

		Vector4 color;//ライトの色
		Vector3 position;//ライトの位置
		float intensity;//輝度
		float radius;// ライトの届く最大距離
		float decay;// 減衰率
		float padding[2];

	};

	struct SpotLight
	{
		Vector4 color;//!<ライトの色
		Vector3 position;//ライトの位置
		float intensity;//輝度
		Vector3 direction;//スポットライトの方向
		float distance;//ライトの届く最大距離
		float decay;//減衰率
		float cosAngle;//スポットライトの余弦
		float cosFalloffStart;//スポットライトの減衰開始角の余弦
		float padding;
	};

	void Initialize(Object3dCommon* object3dCommon);
	void Update();
	void Draw();
	void DebugUpdate();

	void SetParent(Object3d* parentPtr) { parent_ = parentPtr; }

	//setter
	void SetModel(Model* model) { this->model = model; }
	void SetScale(const Vector3& scale) { this->transform.scale = scale; }
	void SetRotate(const Vector3& rotate) { this->transform.rotate = rotate; }
	void SetTranslate(const Vector3& translate) { this->transform.translate = translate; }
	void SetModel(const std::string& filePath);
	void SetCamera(Camera* camera) { this->camera = camera; }
	

	//getter
	const Vector3& GetScale()const { return transform.scale; }
	const Vector3& GetRotate()const { return transform.rotate; }
	const Vector3& GetTranslate()const { return transform.translate; }
	void SetIsUseEnvironmentMap(bool isUse) { this->isUseEnvironmentMap = isUse; }
	void SetEnvironmentCoefficient(float coefficient);
	void SetEnvironmentTexture(const std::string& filePath);


private:

	Object3d* parent_ = nullptr;

	Object3dCommon* object3dCommon = nullptr;

	bool isUseEnvironmentMap;

	//バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResources;
	//バッファリソース内のデータを指すポインタ
	TransformationMatrix* transformationMatrixData = nullptr;

	//平行光源用のリソースを作る。今回はカラー１つ分のサイズを用意する
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource;
	//データを書き込む
	DirectionalLight* directionalLightData = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> cameraResource;
	CameraForGPU* cameraData = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> pointResource;
	PointLight* pointLightData = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> spotResource;
	SpotLight* spotLightData = nullptr;

	//平行光源の切り替え	
	bool useLighting = false;

	Transform transform;

	DirectXCommon* dxCommon_ = nullptr;

	Model* model = nullptr;

	Camera* camera = nullptr;
	
	 float reflectIntensity = 0.5f;
	 float outerAngle = 60.0f; // 外側角度
	 float innerAngle = 30.0f; // 内側角度
	
	//座標返還行列データ作成
	void CreateTransformationMatrixData();
	//平行光源データ作成
	void CreateDirectionalLightData();

};