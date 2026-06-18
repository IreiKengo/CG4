#include "Game.h"
#include "Logger.h"
#include "StringUtility.h"
#include "Sprite.h"
#include "Object3d.h"
#include "Camera.h"
#include "ParticleEmitter.h"
#include <dbghelp.h>
#include <strsafe.h>
#include "D3DResourceLeakChecker.h"
#include <filesystem>
#include "Skybox.h"
#include "ParticleManager.h"
#include"TextureManager.h"
#include "Object3dCommon.h"
#include "SkyboxCommon.h"
#include "SpriteCommon.h"  // 今後使うときのために一緒に入れておくと安全です
#include "ImguiManager.h"

#pragma comment(lib,"Dbghelp.lib")

using namespace StringUtility;
using namespace Logger;


D3DResourceLeakChecker leakCheck;



void Game::Initialize()
{


	//基底クラスの初期化処理
	Framework::Initialize();

#pragma region カメラの初期化

	camera = new Camera();
	camera->SetRotate({ 0.0f,0.0f,0.0f });
	camera->SetTranslate({ 0.0f,4.0f,-23.0f });
#pragma endregion 

#pragma region スプライト関連

	


	sprite = new Sprite();

	std::string texturePath = "resources/uvChecker.png";
	sprite->Initialize(spriteCommon, texturePath);

#pragma endregion

#pragma region パーティクル
	ParticleManager::GetInstance()->SetCamera(camera);


	ParticleManager::GetInstance()->CreateParticleGroup(
		"Cylinder",              //新しい名前にする
		"resources/gradationLine.png", //使いたい画像のパス
		ParticleManager::ParticleMeshType::Cylinder
	);


	particleCylinder = new ParticleEmitter(
		"Cylinder",
		Vector3{ 0.0f, 0, 0 },    // 位置を少しずらすと見やすいです
		1,                        // 発生数
		5.0f                      // 発生頻度
	);

	

#pragma endregion

#pragma region スカイボックス

	skyboxCommon->SetDefaultCamera(camera);

	skybox = new Skybox();
	std::string skyboxDdsPath = "resources/rostock_laage_airport_4k.dds";
	skybox->Initialize(skyboxCommon, skyboxDdsPath);

#pragma endregion

#pragma region オブジェクト関係

	
	object3dCommon->SetDefaultCamera(camera);

	for (int i = 0; i < 2; ++i)
	{

		object[i] = new Object3d();

		std::string modelPath;
		if (i % 2 == 0) {
			modelPath = "terrain.obj";
		} else {

			modelPath = "axis.obj";
		}

		object[i]->Initialize(object3dCommon);
		object[i]->SetModel(modelPath);

		object[i]->SetEnvironmentTexture(skyboxDdsPath);
		object[i]->SetIsUseEnvironmentMap(false);

	}

#pragma endregion

	


}

void Game::Finalize()
{

	

	delete particleCylinder;
	particleCylinder = nullptr;

	for (uint32_t i = 0; i < 2; ++i)
	{
		delete object[i];
		object[i] = nullptr;
	}

	//Sprite解放

	

	delete sprite;
	sprite = nullptr;	

	delete skybox;
	skybox = nullptr;

	delete camera;
	camera = nullptr;
	

	//基底クラスの終了処理
	Framework::Finalize();
}

void Game::Update()
{

	//基底クラスの更新処理
	Framework::Update();

	camera->DebugUpdate();

	//sprite->DebugUpdate();
	for (uint32_t i = 0; i < 2; ++i)
	{
		object[i]->DebugUpdate();
	}
	//カメラの更新
	camera->Update();

	//sprite->Update();

	object[0]->SetTranslate({ -1.0f,-1.0f,0.0f });
	object[1]->SetTranslate({ 1.0f,-1.0f,0.0f });
	for (uint32_t i = 0; i < 2; ++i)
	{
		object[i]->Update();
	}

	float deltaTime = 1.0f / 60.0f; // 本来は実時間計測

	skybox->Update();

	
	
	particleCylinder->Update(deltaTime);
	ParticleManager::GetInstance()->Update();


}

void Game::Draw()

{

	//DirectXの描画基準。全ての描画に共通宇のグラッフィックスコマンドを積む
	dxCommon->PreDraw();

	skybox->Draw();

	//3Dオブジェクトの描画準備。3Dオブジェクトの描画に共通のグラフィックスコマンドを積む
	object3dCommon->ScreenCommon();


	//全てのObject3d個々の描画
	for (uint32_t i = 0; i < 2; ++i)
	{
		object[i]->Draw();

	}

	//Spriteの描画基準。Spriteの描画の共通のグラッフィックスコマンドを積む
	//spriteCommon->ScreenCommon();


	//Spriteの描画
	//sprite->Draw();
	 

	ParticleManager::GetInstance()->Draw();

	imgui->End();    // ImGui終了
	imgui->Draw();   // 描画

	//描画後処理
	dxCommon->PostDraw();

	TextureManager::GetInstance()->ReleaseIntermediateResources();

}

