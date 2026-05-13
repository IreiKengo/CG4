#pragma once
#include "DirectXCommon.h"


class SkyboxCommon
{

public:

	void Initialize(DirectXCommon* dxCommon);
	//共通画面設定
	void ScreenCommon();


	DirectXCommon* GetDxCommon()const { return dxCommon_; }


private:

	//ルートシグネチャ
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;

	DirectXCommon* dxCommon_;


	//ルートシグネチャの作成
	void CreateRootSignature();
	//グラフィックスパイプラインの作成
	void CreateGraphicsPipeline();



	//右面。描画インデックスは[0,1,2][2,1,3]で内側を向く
	vertexData[0].position = { 1.0f,1.0f,1.0f,1.0f };
	vertexData[1].position = { 1.0f,1.0f,-1.0f,1.0f };
	vertexData[2].position = { 1.0f,-1.0f,1.0f,1.0f };
	vertexData[3].position = { 1.0f,-1.0f,-1.0f,1.0f };
	//左面。描画インデックスは[4,5,6][6,5,7]
	vertexData[4].position = { -1.0f,1.0f,-1.0f,1.0f };
	vertexData[5].position = { -1.0f,1.0f,1.0f,1.0f };
	vertexData[6].position = { -1.0f,-1.0f,-1.0f,1.0f };
	vertexData[7].position = { -1.0f,-1.0f,1.0f,1.0f };
	//前面。描画インデックスは[8,9,10][10,9,11]
	vertexData[8].position = { -1.0f,1.0f,1.0f,1.0f };
	vertexData[9].position = { 1.0f,1.0f,1.0f,1.0f };
	vertexData[10].position = { -1.0f,-1.0f,1.0f,1.0f };
	vertexData[11].position = { 1.0f,-1.0f,1.0f,1.0f };

	//後面。描画インデックスは[12,13,14][14,13,15]
	vertexData[12].position = { 1.0f,1.0f,-1.0f,1.0f };
	vertexData[13].position = { -1.0f,1.0f,-1.0f,1.0f };
	vertexData[14].position = { 1.0f,-1.0f,-1.0f,1.0f };
	vertexData[15].position = { -1.0f,-1.0f,-1.0f,1.0f };
	//上面。描画インデックスは[16,17,18][18,17,19]
	vertexData[16].position = { -1.0f,1.0f,-1.0f,1.0f };
	vertexData[17].position = { 1.0f,1.0f,-1.0f,1.0f };
	vertexData[18].position = { -1.0f,1.0f,1.0f,1.0f };
	vertexData[19].position = { 1.0f,1.0f,1.0f,1.0f };
	//下面。描画インデックスは[20,21,22][22,21,23]
	vertexData[20].position = { -1.0f,-1.0f,1.0f,1.0f };
	vertexData[21].position = { 1.0f,-1.0f,1.0f,1.0f };
	vertexData[22].position = { -1.0f,-1.0f,-1.0f,1.0f };
	vertexData[23].position = { 1.0f,-1.0f,-1.0f,1.0f };


};
