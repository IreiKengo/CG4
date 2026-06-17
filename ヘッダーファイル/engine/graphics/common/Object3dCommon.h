#pragma once
#include "DirectXCommon.h"

class Camera;

//3Dオブジェクト共通部
class Object3dCommon
{

public:

	void Initialize(DirectXCommon* dxCommon);
	//共通画面設定
	void ScreenCommon();

	DirectXCommon* GetCommon()const { return dxCommon_; }

	//setter
	void SetDefaultCamera(Camera* camera) { this->defaultCamera = camera; }

	//getter
	Camera* GetDefaultCamera()const { return defaultCamera; }
	ID3D12PipelineState* GetPipelineState() const { return graphicsPipelineState.Get(); }
	ID3D12PipelineState* GetPipelineStateEnv() const { return graphicsPipelineStateEnvironment.Get(); }

private:

	//ルートシグネチャ
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateEnvironment = nullptr;

	

	DirectXCommon* dxCommon_;

	Camera* defaultCamera = nullptr;

	//ルートシグネチャの作成
	void CreateRootSignature();
	//グラフィックスパイプラインの作成
	void CreateGraphicsPipeline();

};
