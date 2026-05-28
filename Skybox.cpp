#include "Skybox.h"
#include "SkyboxCommon.h"
#include "DirectXCommon.h"
#include "Matrix4x4Math.h"
#include "Camera.h"
#include "TextureManager.h"
#include "SrvManager.h"

using namespace math;

void Skybox::Initialize(SkyboxCommon* skyboxCommon, std::string filePath)
{

	skyboxCommon_ = skyboxCommon;

	dxCommon_ = skyboxCommon->GetDxCommon();

	srvManager_ = dxCommon_->GetSrvManager();

	textureFilePath_ = filePath;
	textureIndex = 0;

	CreateVertexData();
	CreateMaterialData();
	CreateTransformationMatrixData();
	
	transform_ = { {1.0f,1.0f,1.0f,},{0.0f,0.0f,1.0f},{0.0f,0.0f,0.0f} };

	textureIndex = TextureManager::GetInstance()->GetTextureIndexByFilePath(filePath);

}

void Skybox::Update()
{
	camera = skyboxCommon_->GetDefaultCamera();
	
	transform_.translate = camera->GetTranslate();

	Matrix4x4 worldMatrix = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	Matrix4x4 wvpMatrix = Multiply(worldMatrix, camera->GetViewProjectionMatrix());

	transformationMatrixData_->World = worldMatrix;
	transformationMatrixData_->WVP = wvpMatrix;

}


void Skybox::Draw()
{

	skyboxCommon_->ScreenCommon();

	dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);//VBVを設定
	dxCommon_->GetCommandList()->IASetIndexBuffer(&indexBufferView_);//IBVを設定

	//マテリアルCBufferの場所を設定
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	//TransformationMatrixCBufferの場所を設定
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResources_->GetGPUVirtualAddress());
	srvManager_->SetGraphicsRootDescriptorTable(2, textureIndex);
	dxCommon_->GetCommandList()->DrawIndexedInstanced(36, 1, 0, 0, 0);
}

void Skybox::DebugUpdate()
{

}



void Skybox::CreateTransformationMatrixData()
{

	//座標変換行列リソースを作る
	transformationMatrixResources_ = dxCommon_->CreateBufferResource(sizeof(TransformationMatrix));

	//書き込むためのアドレスを取得
	transformationMatrixResources_->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData_));


	//単位行列を書き込んでいく
	transformationMatrixData_->WVP = MakeIdentity4x4();
	transformationMatrixData_->World = MakeIdentity4x4();

}

void Skybox::CreateVertexData()
{

	VertexData vertices[8] = {
		{ { -1.0f,  1.0f, -1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f } }, // [0] 左上奥
		{ {  1.0f,  1.0f, -1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f } }, // [1] 右上奥
		{ { -1.0f, -1.0f, -1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f } }, // [2] 左下奥
		{ {  1.0f, -1.0f, -1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f } }, // [3] 右下奥
		{ { -1.0f,  1.0f,  1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f } }, // [4] 左上手前
		{ {  1.0f,  1.0f,  1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f } }, // [5] 右上手前
		{ { -1.0f, -1.0f,  1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f } }, // [6] 左下手前
		{ {  1.0f, -1.0f,  1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f } }, // [7] 右下手前
	};


	size_t vertexSizeInBytes = sizeof(VertexData) * 8;

	vertexResource_ = dxCommon_->CreateBufferResource(vertexSizeInBytes);

	VertexData* vertexData = nullptr;
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	std::memcpy(vertexData, vertices, vertexSizeInBytes);
	vertexResource_->Unmap(0, nullptr);

	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = static_cast<UINT>(vertexSizeInBytes);
	vertexBufferView_.StrideInBytes = sizeof(VertexData);


	
	uint32_t indices[36] = {
		1, 0, 3,  3, 0, 2, // 後面
		4, 5, 6,  6, 5, 7, // 前面
		0, 4, 2,  2, 4, 6, // 左面
		5, 1, 7,  7, 1, 3, // 右面
		0, 1, 4,  4, 1, 5, // 上面
		6, 7, 2,  2, 7, 3  // 下面
	};

	size_t indexSizeInBytes = sizeof(uint32_t) * 36;
	indexResource_ = dxCommon_->CreateBufferResource(indexSizeInBytes);

	uint32_t* indexData = nullptr;
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
	std::memcpy(indexData, indices, indexSizeInBytes);
	indexResource_->Unmap(0, nullptr);

	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	indexBufferView_.SizeInBytes = static_cast<UINT>(indexSizeInBytes);
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

}

void Skybox::CreateMaterialData()
{

	materialResource_ = dxCommon_->CreateBufferResource(sizeof(Material));
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	materialData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };

}
