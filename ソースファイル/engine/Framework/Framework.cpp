#include "Framework.h"
#include "Input.h"
#include "WinApp.h"
#include "DirectXCommon.h"
#include "ImguiManager.h"
#include "Sound.h"
#include "Logger.h"
#include "StringUtility.h"
#include <strsafe.h>
#include <dbghelp.h>
#include "TextureManager.h"
#include "ModelManager.h"
#include "ParticleManager.h"
#include "SpriteCommon.h"
#include "Object3dCommon.h"
#include "SkyboxCommon.h"


using namespace StringUtility;
using namespace Logger;

static LONG WINAPI ExportDump(EXCEPTION_POINTERS* exception)
{

	SYSTEMTIME time;
	GetLocalTime(&time);
	wchar_t filePath[MAX_PATH] = { 0 };
	CreateDirectory(L"./Dumps", nullptr);
	StringCchPrintfW(filePath, MAX_PATH, L"./Dumps/%04d-%02d%02d-%02d%02d.dmp", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute);
	HANDLE dumpFileHandle = CreateFile(filePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);
	//processId(このexeのId)とクラッシュ(例外)の発生したthreadIdを取得
	DWORD processId = GetCurrentProcessId();
	DWORD threadId = GetCurrentThreadId();
	//設定情報を入力
	MINIDUMP_EXCEPTION_INFORMATION minidumpInformation{ 0 };
	minidumpInformation.ThreadId = threadId;
	minidumpInformation.ExceptionPointers = exception;
	minidumpInformation.ClientPointers = TRUE;
	//Dumpを入力。MiniDumpNormalは最低限の情報を出力するフラグ
	MiniDumpWriteDump(GetCurrentProcess(), processId, dumpFileHandle, MiniDumpNormal, &minidumpInformation, nullptr, nullptr);
	//他に関連づけられているSEH例外ハンドラがあれば実行。通常はプロセスを終了する

	return EXCEPTION_EXECUTE_HANDLER;

}

void Framework::Initialize()
{

	Logger::Initialize();
	Log("Hello DirectX!\n");
	Log(
		ConvertString(
			std::format(
				L"clientSize:{},{}\n",
				WinApp::kClientWidth,
				WinApp::kClientHeight
			)
		)
	);

	SetUnhandledExceptionFilter(ExportDump);

#pragma region WindowsAPIの初期化

	//WindowsAPIの初期化
	winApp = new WinApp();
	winApp->Initialize();

#pragma endregion

#pragma region DirectXの初期化

	//DirectXの初期化
	dxCommon = new DirectXCommon();
	dxCommon->Initialize(winApp);

#pragma endregion

#pragma region DirectInputの初期化

	//入力の初期化
	input = new Input();
	input->Initialize(winApp);

#pragma endregion

#pragma region Sound

	sound = new Sound();

	sound->Initialize("resources/sound/fanfare.mp3");

#pragma endregion

#pragma region Imguiの初期化

	imgui = new ImguiManager();

	imgui->Initialize(winApp, dxCommon);

#pragma endregion


	//テクスチャマネージャの初期化
	TextureManager::GetInstance()->Initialize(dxCommon);
	//3Dモデルマネージャの初期化
	ModelManager::GetInstance()->Initialize(dxCommon);
	ParticleManager::GetInstance()->Initialize(dxCommon);


	//スプライト共通部の初期化
	spriteCommon = new SpriteCommon;
	spriteCommon->Initialize(dxCommon);


	//3Dオブジェクト共通部の初期化
	object3dCommon = new Object3dCommon;
	object3dCommon->Initialize(dxCommon);


	skyboxCommon = new SkyboxCommon();
	skyboxCommon->Initialize(dxCommon);

}

void Framework::Update()
{

	if (winApp->ProcessMessage())
	{
		endRequest_ = true;
	}

	//入力の更新
	input->Update();

	imgui->Begin();

	//ImGui::ShowDemoWindow();
	
	if (input->TriggerKey(DIK_W))
	{
		sound->SoundPlayWave();
	}

}

void Framework::Run()
{

	Initialize();

	while (true)
	{

		Update();

		if (IsEndRequest())
		{
			break;
		}

		Draw();
	}
	Finalize();
}

void Framework::Finalize()
{

	imgui->Finalize();
	delete imgui;
	imgui = nullptr;

	sound->SoundUnload();
	sound->Finalize();
	delete sound;
	sound = nullptr;

	//入力解放
	delete input;
	input = nullptr;

	ParticleManager::GetInstance()->Finalize();

	//Dモデルマネージャの終了
	ModelManager::GetInstance()->Finalize();

	//TextureManager解放
	TextureManager::GetInstance()->Finalize();

	delete object3dCommon;
	delete skyboxCommon;

	//SpriteCommon解放
	delete spriteCommon;

	//DirectX解放
	delete dxCommon;
	dxCommon = nullptr;

	//WindowsAPIの終了処理
	winApp->Finalize();

	//WindowsAPIの解放
	delete winApp;
	winApp = nullptr;

	
}

