#include "Object3d.h"
#include "Object3dCommon.h"
#include "Matrix4x4Math.h"
#include "TextureManager.h"
#include "Model.h"
#include "ModelManager.h"
#include "Camera.h"
#include <numbers>
#include "ImguiManager.h"

using namespace math;


void Object3d::Initialize(Object3dCommon* object3dCommon)
{

	//引数で受け取ってメンバ変数に記録する
	this->object3dCommon = object3dCommon;

	dxCommon_ = object3dCommon->GetCommon();

	this->camera = object3dCommon->GetDefaultCamera();

	CreateTransformationMatrixData();

	CreateDirectionalLightData();

	//Transform変数を作る
	transform = { {1.0f,1.0f,1.0f},{0.0f,3.156f,0.0f},{0.0f,0.0f,0.0f} };


}

void Object3d::Update()
{

	//transform.rotate.y += 0.03f;
	
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
	Matrix4x4 worldViewProjectionMatrix;
	if (camera)
	{
		const Matrix4x4& viewProjectionMatrix = camera->GetViewProjectionMatrix();
		worldViewProjectionMatrix = Multiply(worldMatrix, viewProjectionMatrix);
	} else
	{
		worldViewProjectionMatrix = worldMatrix;
	}
	transformationMatrixData->WVP = model->GetLocalMatrix() * worldViewProjectionMatrix;
	transformationMatrixData->World = model->GetLocalMatrix() * worldMatrix;
	transformationMatrixData->WorldInverseTranspose = Transpose(Inverse(worldMatrix));



}

void Object3d::Draw()
{


	//座標変換行列CBufferの場所を設定
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResources->GetGPUVirtualAddress());
	//平行光源CBufferの場所を設定
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(4, cameraResource->GetGPUVirtualAddress());
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(5, pointResource->GetGPUVirtualAddress());
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(6, spotResource->GetGPUVirtualAddress());

	//3Dモデルが割り当てられていれば描画する
	if (model)
	{
		model->Draw();
	}


}

void Object3d::DebugUpdate()
{

#ifdef USE_IMGUI
	char windowName[64];
	sprintf_s(windowName, "Light[%p]", this);

	ImGui::Begin(windowName);

	ImGui::PushID(this);

	ImGui::Checkbox("useLighting", &useLighting);

	const char* lightingItems[] = {
"Phong",
"Blinn-Phong",
"PointLight",
"SpotLight"
	};

	Model::Material* material = model->GetMaterial();

	
	ImGui::Combo(
		"LightingModel",
		&material->lightingModel,
		lightingItems,
		IM_ARRAYSIZE(lightingItems)
	);

	

	ImGui::ColorEdit4("LightColor", &(*directionalLightData).color.x);
	ImGui::SliderFloat3("LightDirection", &directionalLightData->direction.x, -1.0f, 1.0f);
	ImGui::DragFloat("intensity", &directionalLightData->intensity, 0.1f);

	ImGui::DragFloat3("PointLightPosition", &pointLightData->position.x, 0.1f);
	ImGui::SliderFloat("PointLightRadius", &pointLightData->radius, 0.0f, 10.0f);
	ImGui::SliderFloat("PointLightDecay", &pointLightData->decay, 0.0f, 10.0f);



	ImGui::DragFloat3("spotLightPosition", &spotLightData->position.x, 0.1f);
	ImGui::DragFloat3("spotLightDirection", &spotLightData->direction.x, 0.01f);
	spotLightData->direction =
		Normalize(spotLightData->direction);

	ImGui::SliderFloat("spotLightDistance", &spotLightData->distance, 0.0f, 20.0f);
	ImGui::DragFloat("spotLightDecay", &spotLightData->decay, 0.1f, 10.0f);

	static float outerAngle = 60.0f; // 外側角度
	static float innerAngle = 30.0f; // 内側角度


	ImGui::SliderFloat(
		"Outer Angle",
		&outerAngle,
		1.0f,
		89.0f
	);

	ImGui::SliderFloat(
		"Inner Angle",
		&innerAngle,
		1.0f,
		outerAngle
	);

	// degree -> cos に変換
	spotLightData->cosAngle =
		std::cos(
			outerAngle *
			std::numbers::pi_v<float> /
			180.0f
		);

	spotLightData->cosFalloffStart =
		std::cos(
			innerAngle *
			std::numbers::pi_v<float> /
			180.0f
		);

	ImGui::PopID();

	ImGui::End();
	//useLighting ? materialData->enableLighting = true : materialData->enableLighting = false;

	if (model) {
		model->SetEnableLighting(useLighting);
	}

#endif 

}

void Object3d::SetModel(const std::string& filePath)
{

	//モデルを検索してセットする
	model = ModelManager::GetInstance()->FindModel(filePath);

}


void Object3d::CreateTransformationMatrixData()
{
	//座標変換行列リソースを作る
	transformationMatrixResources = dxCommon_->CreateBufferResource(sizeof(TransformationMatrix));

	//書き込むためのアドレスを取得
	transformationMatrixResources->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData));


	//単位行列を書き込んでいく
	transformationMatrixData->WVP = MakeIdentity4x4();
	transformationMatrixData->World = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
	transformationMatrixData->WorldInverseTranspose = Transpose(Inverse(transformationMatrixData->World));

}

void Object3d::CreateDirectionalLightData()
{

	directionalLightResource = dxCommon_->CreateBufferResource(sizeof(DirectionalLight));

	//書き込むためのアドレスを取得
	directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));
	//デフォルト値はとりあえず以下のようにしておく
	directionalLightData->color = { 1.0f,1.0f,1.0f,1.0f };
	directionalLightData->direction = { 0.0f,-1.0f,0.0f };
	directionalLightData->intensity = 0.0f;

	cameraResource = dxCommon_->CreateBufferResource(sizeof(CameraForGPU));

	cameraResource->Map(0, nullptr, reinterpret_cast<void**>(&cameraData));
	cameraData->worldPosition = camera->GetTranslate();

	//PointLight用リソース
	pointResource = dxCommon_->CreateBufferResource(sizeof(PointLight));

	pointResource->Map(0, nullptr, reinterpret_cast<void**>(&pointLightData));

	pointLightData->color = { 1.0f,1.0f,1.0f,1.0f };
	pointLightData->intensity = 0.0f;
	pointLightData->position = { 0.0f,2.0f,0.0f };
	pointLightData->radius = 5.0f;
	pointLightData->decay = 1.0f;

	//SpotLight用リソース
	spotResource = dxCommon_->CreateBufferResource(sizeof(SpotLight));

	spotResource->Map(0, nullptr, reinterpret_cast<void**>(&spotLightData));

	spotLightData->color = { 1.0f,1.0f,1.0f,1.0f };
	spotLightData->position = { -0.2f,-0.5f,0.0f };
	spotLightData->distance = 7.0f;
	spotLightData->direction = Normalize({ -1.0f,-1.0f,0.0f });
	spotLightData->intensity = 4.0f;
	spotLightData->decay = 2.0f;
	spotLightData->cosAngle = std::cos(std::numbers::pi_v<float> / 3.0f);
	spotLightData->cosFalloffStart = std::cos(std::numbers::pi_v<float> / 6.0f);

}
