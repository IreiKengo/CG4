#pragma once
#include"Vector3.h"
#include"Vector4.h"
#include <wrl.h>
#include <d3d12.h>
#include <cstdint>
#include "Transform.h"
#include "Matrix4x4.h"
#include <string>

class DirectXCommon;
class SkyboxCommon;
class Camera;
class SrvManager;

class Skybox
{
public:

	//頂点データ
	struct VertexData {
		Vector4 position;
		Vector3 texCoord;
	};
	//マテリアルデータ
	struct Material
	{
		Vector4 color;
	};
	//座標変換行列データ
	struct TransformationMatrix
	{
		Matrix4x4 WVP;
		Matrix4x4 World;
	};


	void Initialize(SkyboxCommon* skyboxCommon, std::string filePath);
	void Update();
	void Draw();

	void DebugUpdate();

	

private:

	SkyboxCommon* skyboxCommon_ = nullptr;
	DirectXCommon* dxCommon_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_ = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_ = nullptr;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};

	//マテリアルリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_ = nullptr;
	Material* materialData_ = nullptr;


	//バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResources_ = nullptr;
	//バッファリソース内のデータを指すポインタ
	TransformationMatrix* transformationMatrixData_ = nullptr;

	Transform transform_;

	Camera* camera = nullptr;
	SrvManager* srvManager_ = nullptr;

	std::string textureFilePath_;
	uint32_t textureIndex = 0;



	//座標返還行列データ作成
	void CreateTransformationMatrixData();

	void CreateVertexData();
	void CreateMaterialData();

};
