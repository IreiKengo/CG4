#pragma once
#include <wrl.h>
#include <d3d12.h>

class DirectXCommon;
class Camera;

class SkyboxCommon
{

public:

	void Initialize(DirectXCommon* dxCommon);
	//共通画面設定
	void ScreenCommon();



	DirectXCommon* GetDxCommon()const { return dxCommon_; }

	//setter
	void SetDefaultCamera(Camera* camera) { this->defaultCamera = camera; }

	//getter
	Camera* GetDefaultCamera()const { return defaultCamera; }


private:

	//ルートシグネチャ
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_ = nullptr;

	DirectXCommon* dxCommon_ = nullptr;

	Camera* defaultCamera = nullptr;

	//ルートシグネチャの作成
	void CreateRootSignature();
	//グラフィックスパイプラインの作成
	void CreateGraphicsPipeline();


};
