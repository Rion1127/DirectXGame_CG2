#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <vector>
#include <string>
#include <DirectXMath.h>
///////自作クラス////////
#include "DirectXInput.h"
#include "Viewport.h"
#include "WinAPI.h"
///////////////////////
using namespace DirectX;
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#include <DirectXTex.h>

struct ConstBufferDataMaterial {
	XMFLOAT4 colro;
};
//定数バッファ用データ構造
struct ConstBufferDataTransform {
	XMMATRIX mat;
};

//頂点データ構造体
struct Vertex {
	XMFLOAT3 pos;
	XMFLOAT2 uv;
};

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
#pragma region WindowsAPI初期化処理
	//-+-+-+-+-+-+-+-+-+-+-+-+-//
	//WindowsAPI初期化処理 ここから//
	//-+-+-+-+-+-+-+-+-+-+-+-+-//
	WinAPI winAPI;
	winAPI.UpdataWindowsAPI();
	//-+-+-+-+-+-+-+-+-+-+-+-+-//
	//WindowsAPI初期化処理 ここまで//
	//-+-+-+-+-+-+-+-+-+-+-+-+-//
#pragma endregion

#pragma region DirectX初期化処理
		//-+-+-+-+-+-+-+-+-+-+-+-//
		//DirectX初期化処理　ここから//
		//-+-+-+-+-+-+-+-+-+-+-+-//

#ifdef _DEBUG
//デバッグレイヤーをオンに
	ID3D12Debug* debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		debugController->EnableDebugLayer();
	}
#endif

	HRESULT result;
	ID3D12Device* device = nullptr;
	IDXGIFactory7* dxgiFactory = nullptr;
	IDXGISwapChain4* swapChain = nullptr;
	ID3D12CommandAllocator* commandAllocator = nullptr;
	ID3D12GraphicsCommandList* commandList = nullptr;
	ID3D12CommandQueue* commandQueue = nullptr;
	ID3D12DescriptorHeap* rtvHeap = nullptr;

	// DXGIファクトリーの生成
	result = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
	assert(SUCCEEDED(result));
	// アダプターの列挙用
	std::vector<IDXGIAdapter4*> adapters;
	// ここに特定の名前を持つアダプターオブジェクトが入る
	IDXGIAdapter4* tmpAdapter = nullptr;
	// パフォーマンスが高いものから順に、全てのアダプターを列挙する
	for (UINT i = 0;
		dxgiFactory->EnumAdapterByGpuPreference(i,
			DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
			IID_PPV_ARGS(&tmpAdapter)) != DXGI_ERROR_NOT_FOUND;
		i++) {
		// 動的配列に追加する
		adapters.push_back(tmpAdapter);
	}

	// 妥当なアダプタを選別する
	for (size_t i = 0; i < adapters.size(); i++) {
		DXGI_ADAPTER_DESC3 adapterDesc;
		// アダプターの情報を取得する
		adapters[i]->GetDesc3(&adapterDesc);
		// ソフトウェアデバイスを回避
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			// デバイスを採用してループを抜ける
			tmpAdapter = adapters[i];
			break;
		}
	}

	// 対応レベルの配列
	D3D_FEATURE_LEVEL levels[] = {
	D3D_FEATURE_LEVEL_12_1,
	D3D_FEATURE_LEVEL_12_0,
	D3D_FEATURE_LEVEL_11_1,
	D3D_FEATURE_LEVEL_11_0,
	};

	D3D_FEATURE_LEVEL featureLevel;
	for (size_t i = 0; i < _countof(levels); i++) {
		// 採用したアダプターでデバイスを生成
		result = D3D12CreateDevice(tmpAdapter, levels[i],
			IID_PPV_ARGS(&device));
		if (result == S_OK) {
			// デバイスを生成できた時点でループを抜ける
			featureLevel = levels[i];
			break;
		}
	}

	// コマンドアロケータを生成
	result = device->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&commandAllocator));
	assert(SUCCEEDED(result));
	// コマンドリストを生成
	result = device->CreateCommandList(0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		commandAllocator, nullptr,
		IID_PPV_ARGS(&commandList));
	assert(SUCCEEDED(result));

	//コマンドキューの設定
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	//コマンドキューを生成
	result = device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue));
	assert(SUCCEEDED(result));

	// スワップチェーンの設定
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.Width = 1280;
	swapChainDesc.Height = 720;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 色情報の書式
	swapChainDesc.SampleDesc.Count = 1; // マルチサンプルしない
	swapChainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER; // バックバッファ用
	swapChainDesc.BufferCount = 2; // バッファ数を2つに設定
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // フリップ後は破棄
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	// スワップチェーンの生成
	result = dxgiFactory->CreateSwapChainForHwnd(
		commandQueue, winAPI.hwnd, &swapChainDesc, nullptr, nullptr,
		(IDXGISwapChain1**)&swapChain);
	assert(SUCCEEDED(result));

	// デスクリプタヒープの設定
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; // レンダーターゲットビュー
	rtvHeapDesc.NumDescriptors = swapChainDesc.BufferCount; // 裏表の2つ
	// デスクリプタヒープの生成
	device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap));

	// バックバッファ
	std::vector<ID3D12Resource*> backBuffers;
	backBuffers.resize(swapChainDesc.BufferCount);

	// スワップチェーンの全てのバッファについて処理する
	for (size_t i = 0; i < backBuffers.size(); i++) {
		// スワップチェーンからバッファを取得
		swapChain->GetBuffer((UINT)i, IID_PPV_ARGS(&backBuffers[i]));
		// デスクリプタヒープのハンドルを取得
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap->GetCPUDescriptorHandleForHeapStart();
		// 裏か表かでアドレスがずれる
		rtvHandle.ptr += i * device->GetDescriptorHandleIncrementSize(rtvHeapDesc.Type);
		// レンダーターゲットビューの設定
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
		// シェーダーの計算結果をSRGBに変換して書き込む
		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		// レンダーターゲットビューの生成
		device->CreateRenderTargetView(backBuffers[i], &rtvDesc, rtvHandle);
	}

	// フェンスの生成
	ID3D12Fence* fence = nullptr;
	UINT64 fenceVal = 0;
	result = device->CreateFence(fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));



	//定数バッファの生成
	ID3D12Resource* constBuffTransform = nullptr;
	ConstBufferDataTransform* constMapTransform = nullptr;
	/*XMMATRIX old;
	XMMATRIX a;*/
	//ビュー変換行列
	XMMATRIX matView;
	XMFLOAT3 eye(0, 0, -20);	//視点座標
	XMFLOAT3 target(0, 0, 0);	//注視点座標
	XMFLOAT3 up(0, 1, 0);		//上方向ベクトル
	matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));

	//ワールド変換行列
	XMMATRIX matWorld(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);
	//スケーリング行列
	XMMATRIX matScale;
	matScale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	matWorld *= matScale;	//ワールド座標にスケーリングを反映
	//回転行列
	XMMATRIX matRot;
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(0.0f));	//z軸まわりに0度回転
	matRot *= XMMatrixRotationX(XMConvertToRadians(15.0f));	//x軸まわりに15度回転
	matRot *= XMMatrixRotationY(XMConvertToRadians(30.0f));	//y軸まわりに30度回転
	matWorld *= matRot;	//ワールド行列に回転を反映
	//平行移動
	XMMATRIX matTrans;
	matTrans = XMMatrixTranslation(0.0f, 0, 0);
	matWorld *= matTrans;	//ワールド行列に平行移動を反映
	{
		//ヒープ設定
		D3D12_HEAP_PROPERTIES cbHeapProp{};
		cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;			//GPUへの転送用
		//リソース設定
		D3D12_RESOURCE_DESC cbResourceDesc{};
		cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		cbResourceDesc.Width = (sizeof(ConstBufferDataTransform) + 0xff) & ~0xff;	//256バイトアラインメント
		cbResourceDesc.Height = 1;
		cbResourceDesc.DepthOrArraySize = 1;
		cbResourceDesc.MipLevels = 1;
		cbResourceDesc.SampleDesc.Count = 1;
		cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		//定数バッファの生成
		result = device->CreateCommittedResource(
			&cbHeapProp,
			D3D12_HEAP_FLAG_NONE,
			&cbResourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&constBuffTransform));
		assert(SUCCEEDED(result));

		//定数バッファのマッピング
		result = constBuffTransform->Map(0, nullptr, (void**)&constMapTransform);	//マッピング
		assert(SUCCEEDED(result));

		//単位行列を代入
		constMapTransform->mat = XMMatrixOrthographicOffCenterLH(
			0.0f, winAPI.GetWinWidth(),
			winAPI.GetWinHeight(), 0.0f,
			0.0f, 1.0f);
		//old = XMMatrixIdentity();
		////平行投影行列の計算
		//a = XMMatrixOrthographicOffCenterLH(
		//	0.0f, winAPI.GetWinWidth(),
		//	winAPI.GetWinHeight(), 0.0f,
		//	0.0f, 1.0f);
		/*constMapTransform->mat.r[0].m128_f32[0] = 2.0f / winAPI.GetWinWidth();
		constMapTransform->mat.r[1].m128_f32[1] = -2.0f / winAPI.GetWinHeight();
		constMapTransform->mat.r[3].m128_f32[0] = -1.0f;
		constMapTransform->mat.r[3].m128_f32[1] = 1.0f;*/
		/*old.r[0].m128_f32[0] = 2.0f / winAPI.GetWinWidth();
		old.r[1].m128_f32[1] = -2.0f / winAPI.GetWinHeight();
		old.r[3].m128_f32[0] = -1.0f;
		old.r[3].m128_f32[1] = 1.0f;*/
	}

	//透視投影行列の計算
	XMMATRIX matProjection = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(45.0f),
		(float)winAPI.GetWinWidth() / winAPI.GetWinHeight(),
		0.1f, 1000.0f
	);
	//定数バッファに転送
	constMapTransform->mat = matWorld * matView * matProjection;

	//キーボードインプット初期化
	DirectXInput::InputIni(winAPI.w, winAPI.hwnd);

	//-+-+-+-+-+-+-+-+-+-+-+-//
	//DirectX初期化処理　ここまで//
	//-+-+-+-+-+-+-+-+-+-+-+-//
#pragma endregion

#pragma region 描画初期化処理
	///////////////////////
	//描画初期化処理　ここから//
	///////////////////////

	Viewport viewport;
	// 頂点データ
	//Vertex vertices[] = {
	//	{{ -0.4f,-0.7f,0.0f} ,{0.0f,1.0f}},//左下
	//	{{ -0.4f,+0.7f,0.0f} ,{0.0f,0.0f}},//左上
	//	{{ +0.4f,-0.7f,0.0f} ,{1.0f,1.0f}},//左下
	//	{{ +0.4f,+0.7f,0.0f} ,{1.0f,0.0f}},//左上
	//};
	//Vertex vertices[] = {
	//{{  0.0f,200.0f,0.0f} ,{0.0f,1.0f}},//左下
	//{{  0.0f,  0.0f,0.0f} ,{0.0f,0.0f}},//左上
	//{{200.0f,200.0f,0.0f} ,{1.0f,1.0f}},//左下
	//{{200.0f,  0.0f,0.0f} ,{1.0f,0.0f}},//左上
	//};

	//Vertex vertices[] = {
	//{{-50.0f,-50.0f,0.0f} ,{0.0f,1.0f}},//左下
	//{{-50.0f, 50.0f,0.0f} ,{0.0f,0.0f}},//左上
	//{{ 50.0f,-50.0f,0.0f} ,{1.0f,1.0f}},//右下
	//{{ 50.0f, 50.0f,0.0f} ,{1.0f,0.0f}},//右上
	//};

	Vertex vertices[] = {
	//前
	{{-5.0f,-5.0f,-5.0f} ,{0.0f,1.0f}},//左下
	{{-5.0f, 5.0f,-5.0f} ,{0.0f,0.0f}},//左上
	{{ 5.0f,-5.0f,-5.0f} ,{1.0f,1.0f}},//右下
	{{ 5.0f, 5.0f,-5.0f} ,{1.0f,0.0f}},//右上
	//後ろ
	{{-5.0f,-5.0f,5.0f} ,{0.0f,1.0f}},//左下
	{{-5.0f, 5.0f,5.0f} ,{0.0f,0.0f}},//左上
	{{ 5.0f,-5.0f,5.0f} ,{1.0f,1.0f}},//右下
	{{ 5.0f, 5.0f,5.0f} ,{1.0f,0.0f}},//右上
	//左
	{{-5.0f,-5.0f,-5.0f} ,{0.0f,1.0f}},//左下
	{{-5.0f,-5.0f, 5.0f} ,{0.0f,0.0f}},//左上
	{{-5.0f, 5.0f,-5.0f} ,{1.0f,1.0f}},//右下
	{{-5.0f, 5.0f, 5.0f} ,{1.0f,0.0f}},//右上
	//右
	{{5.0f,-5.0f,-5.0f} ,{0.0f,1.0f}},//左下
	{{5.0f,-5.0f, 5.0f} ,{0.0f,0.0f}},//左上
	{{5.0f, 5.0f,-5.0f} ,{1.0f,1.0f}},//右下
	{{5.0f, 5.0f, 5.0f} ,{1.0f,0.0f}},//右上
	//下
	{{ 5.0f,-5.0f,-5.0f} ,{0.0f,1.0f}},//左下
	{{ 5.0f,-5.0f, 5.0f} ,{0.0f,0.0f}},//左上
	{{-5.0f,-5.0f,-5.0f} ,{1.0f,1.0f}},//右下
	{{-5.0f,-5.0f, 5.0f} ,{1.0f,0.0f}},//右上
	//上
	{{ 5.0f, 5.0f,-5.0f} ,{0.0f,1.0f}},//左下
	{{ 5.0f, 5.0f, 5.0f} ,{0.0f,0.0f}},//左上
	{{-5.0f, 5.0f,-5.0f} ,{1.0f,1.0f}},//右下
	{{-5.0f, 5.0f, 5.0f} ,{1.0f,0.0f}},//右上
	};

	////インデックスデータ
	//unsigned short indices[] =
	//{
	//	0,1,2,	//三角形1つ目
	//	1,2,3,	//三角形2つ目
	//};

	//インデックスデータ
	unsigned short indices[] =
	{
		//前
		0,1,2,	//三角形1つ目
		1,2,3,	//三角形2つ目
		//後ろ
		4,5,6,
		5,6,7,
		//左
		8,9,10,
		9,10,11,
		//右
		12,13,14,
		13,14,15,
		//下
		16,17,18,
		17,18,19,
		//上
		20,21,22,
		21,22,23
	};

	// 頂点データ全体のサイズ = 頂点データ一つ分のサイズ * 頂点データの要素数
	UINT sizeVB = static_cast<UINT>(sizeof(vertices[0]) * _countof(vertices));

	// 頂点バッファの設定
	D3D12_HEAP_PROPERTIES heapProp{}; // ヒープ設定
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; // GPUへの転送用
	// リソース設定
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeVB; // 頂点データ全体のサイズ
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// 頂点バッファの生成
	ID3D12Resource* vertBuff = nullptr;
	result = device->CreateCommittedResource(
		&heapProp, // ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&resDesc, // リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));
	assert(SUCCEEDED(result));

	// GPU上のバッファに対応した仮想メモリ(メインメモリ上)を取得
	Vertex* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));
	// 全頂点に対して
	for (int i = 0; i < _countof(vertices); i++) {
		vertMap[i] = vertices[i]; // 座標をコピー
	}
	// 繋がりを解除
	vertBuff->Unmap(0, nullptr);

	// 頂点バッファビューの作成
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	// GPU仮想アドレス
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	// 頂点バッファのサイズ
	vbView.SizeInBytes = sizeVB;
	// 頂点1つ分のデータサイズ
	vbView.StrideInBytes = sizeof(vertices[0]);

	ID3DBlob* vsBlob = nullptr; // 頂点シェーダオブジェクト
	ID3DBlob* psBlob = nullptr; // ピクセルシェーダオブジェクト
	ID3DBlob* errorBlob = nullptr; // エラーオブジェクト
	// 頂点シェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"BasicVS.hlsl", // シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "vs_5_0", // エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		&vsBlob, &errorBlob);

	// エラーなら
	if (FAILED(result)) {
		// errorBlobからエラー内容をstring型にコピー
		std::string error;
		error.resize(errorBlob->GetBufferSize());
		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(error.c_str());
		assert(0);
	}

	// ピクセルシェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"BasicPS.hlsl", // シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "ps_5_0", // エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		&psBlob, &errorBlob);

	// エラーなら
	if (FAILED(result)) {
		// errorBlobからエラー内容をstring型にコピー
		std::string error;
		error.resize(errorBlob->GetBufferSize());
		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(error.c_str());
		assert(0);
	}

	viewport.ViewPortIni(vsBlob, psBlob, errorBlob, device);

#pragma region 頂点インデックス
	//インデックスデータ全体のサイズ
	UINT sizeIB = static_cast<UINT>(sizeof(uint16_t) * _countof(indices));

	//リソース設定
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeIB;	//インデックス情報が入る分のサイズ
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//インデックスバッファの生成
	ID3D12Resource* indexBuff = nullptr;
	result = device->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff));

	//インデックスバッファをマッピング
	uint16_t* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexMap);
	//全てのインデックスに対して
	for (int i = 0; i < _countof(indices); i++) {
		indexMap[i] = indices[i];
	}
	//マッピング解除
	indexBuff->Unmap(0, nullptr);

	//インデックスバッファビューの作成
	D3D12_INDEX_BUFFER_VIEW ibView{};
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeIB;

#pragma endregion

#pragma region	シェーダーに色を渡す
	//ヒープ設定
	D3D12_HEAP_PROPERTIES cbHeapProp{};
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	//リソース設定
	D3D12_RESOURCE_DESC cbResourceDesc{};
	cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourceDesc.Width = (sizeof(ConstBufferDataMaterial) + 0xff) & ~0xff;
	cbResourceDesc.Height = 1;
	cbResourceDesc.DepthOrArraySize = 1;
	cbResourceDesc.MipLevels = 1;
	cbResourceDesc.SampleDesc.Count = 1;
	cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	ID3D12Resource* constBufferMaterial = nullptr;
	//定数バッファの生成
	result = device->CreateCommittedResource(
		&cbHeapProp,		//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&cbResourceDesc,	//リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBufferMaterial));
	assert(SUCCEEDED(result));
	//定数バッファのマッピング
	ConstBufferDataMaterial* constMapMaterial = nullptr;
	result = constBufferMaterial->Map(0, nullptr, (void**)&constMapMaterial);	//マッピング
	assert(SUCCEEDED(result));
	//値を書き込むと自動的に転送される
	constMapMaterial->colro = XMFLOAT4(1, 1, 1, 1);

#pragma region テクスチャマッピング
#pragma region 画像読み込み
	//画像読み込み
	TexMetadata metadata{};
	ScratchImage scratchImag{};
	//WICテクスチャダウンロード
	result = LoadFromWICFile(
		L"Resources/KH.jpg",
		WIC_FLAGS_NONE,
		&metadata, scratchImag);

	ScratchImage mipChain{};
	//ミップマップ生成
	result = GenerateMipMaps(
		scratchImag.GetImages(), scratchImag.GetImageCount(), scratchImag.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, mipChain);
	if (SUCCEEDED(result)) {
		scratchImag = std::move(mipChain);
		metadata = scratchImag.GetMetadata();
	}
	//読み込んだディフューズテクスチャをSRGBとして扱う
	metadata.format = MakeSRGB(metadata.format);
#pragma endregion

	////横方向ピクセル数
	//const size_t textureWidth = 256;
	////縦方向ピクセル数
	//const size_t textureHeight = 256;
	////配列の要素数
	//const size_t imageDataCount = textureWidth * textureHeight;
	////画像イメージデータ配列
	//XMFLOAT4* imageData = new XMFLOAT4[imageDataCount];
	////全ピクセルの色を初期化
	//for (size_t i = 0; i < imageDataCount; i++) {
	//	if ((i / 10) % 2 == 0) {
	//		imageData[i].x = 1.0f;	// R
	//		imageData[i].y = 0.0f;	// G
	//		imageData[i].z = 0.0f;	// B
	//		imageData[i].w = 1.0f;	// A
	//	}
	//	else {
	//		imageData[i].x = 0.0f;	// R
	//		imageData[i].y = 0.0f;	// G
	//		imageData[i].z = 0.0f;	// B
	//		imageData[i].w = 0.0f;	// A
	//	}
	//}


	//ヒープ設定
	D3D12_HEAP_PROPERTIES textureHeapProp{};
	textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	textureHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	//リソース設定
	D3D12_RESOURCE_DESC textureResourceDesc{};
	textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResourceDesc.Format = metadata.format;
	textureResourceDesc.Width = metadata.width;
	textureResourceDesc.Height = (UINT)metadata.height;
	textureResourceDesc.DepthOrArraySize = (UINT16)metadata.arraySize;
	textureResourceDesc.MipLevels = (UINT)metadata.mipLevels;
	textureResourceDesc.SampleDesc.Count = 1;

	//テクスチャバッファの生成
	ID3D12Resource* texBuff = nullptr;
	result = device->CreateCommittedResource(
		&textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texBuff));

	//テクスチャバッファにデータ転送
	//result = texBuff->WriteToSubresource(
	//	0,
	//	nullptr,		//前領域へコピー
	//	imageData,		//元データアドレス
	//	sizeof(XMFLOAT4) * textureWidth,	//1ラインサイズ
	//	sizeof(XMFLOAT4) * imageDataCount	//全サイズ
	//);

	for (size_t i = 0; i < metadata.mipLevels; i++) {
		const Image* img = scratchImag.GetImage(i, 0, 0);

		result = texBuff->WriteToSubresource(
			(UINT)i,
			nullptr,
			img->pixels,
			(UINT)img->rowPitch,
			(UINT)img->slicePitch);
		assert(SUCCEEDED(result));

	}

	//SRVの最大個数
	const size_t kMaxSRVCount = 2056;
	//でスクリプタヒープの設定
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//シェーダから見えるように
	srvHeapDesc.NumDescriptors = kMaxSRVCount;

	//設定をもとにSRV用でスクリプタヒープを生成
	ID3D12DescriptorHeap* srvHeap = nullptr;
	result = device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap));
	assert(SUCCEEDED(result));
	//SRVヒープの先頭ハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = srvHeap->GetCPUDescriptorHandleForHeapStart();

	//シェーダリソースビュー設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};			//設定構造体
	srvDesc.Format = resDesc.Format;	//RGBA float
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = resDesc.MipLevels;

	//ハンドルのさす位置にシェーダーリソースビュー作成
	device->CreateShaderResourceView(texBuff, &srvDesc, srvHandle);

	//CBV,SRV,UAVの1古文のサイズを取得
	UINT descriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//ハンドルを1つし占める（SRVの位置）
	srvHandle.ptr += descriptorSize * 1;

	//CBV(コンスタントバッファビュー)の設定
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};

	//定数バッファビュー生成
	device->CreateConstantBufferView(&cbvDesc, srvHandle);

	//深度バッファ
	//リソース設定
	D3D12_RESOURCE_DESC depthResourceDesc{};
	depthResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthResourceDesc.Width = winAPI.GetWinWidth();		//レンダーターゲットに合わせる
	depthResourceDesc.Height = winAPI.GetWinHeight();	//レンダーターゲットに合わせる
	depthResourceDesc.DepthOrArraySize = 1;
	depthResourceDesc.Format = DXGI_FORMAT_D32_FLOAT;	//深度値フォーマット
	depthResourceDesc.SampleDesc.Count = 1;
	depthResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;	//デプスステンシル

	//深度値用ヒーププロパティ
	D3D12_HEAP_PROPERTIES depthHeapProp{};
	depthHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	//深度値のクリア設定
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;		//深度値1.0f（最大値）でクリア
	depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;	//深度値フォーマット

	//リソース生成
	ID3D12Resource* depthBuff = nullptr;
	result = device->CreateCommittedResource(
		&depthHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&depthResourceDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,	//深度値書き込みに使用
		&depthClearValue,
		IID_PPV_ARGS(&depthBuff)
	);

	//深度ビュー用デスクリプタヒープ作成
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1;//深度ビューは1つ
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;//デプスステンシルビュー
	ID3D12DescriptorHeap* dsvHeap = nullptr;
	result = device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap));

	//深度ビュー作成
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;//深度フォーマット
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	device->CreateDepthStencilView(
		depthBuff,
		&dsvDesc,
		dsvHeap->GetCPUDescriptorHandleForHeapStart()
	);

#pragma endregion


#pragma endregion

#pragma endregion


	XMFLOAT4 testColor(0, 0, 0, 0.5f);
	float angle = 0;
	//スケーリング倍率
	XMFLOAT3 scale;
	//回転角
	XMFLOAT3 rotation;
	//座標
	XMFLOAT3 position;
	
	scale = { 1.0f,1.0f,1.0f };
	rotation = { 0.0f,0.0f,0.5f };
	position = { 0.0f,0.0f,0.0f };

	//ゲームループ
	while (true) {
#pragma region ウィンドウメッセージ処理
		///////////////////////
		//ウィンドウメッセージ処理//
		///////////////////////
		// メッセージがある？
		if (PeekMessage(&winAPI.msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&winAPI.msg); // キー入力メッセージの処理
			DispatchMessage(&winAPI.msg); // プロシージャにメッセージを送る
		}
		// ?ボタンで終了メッセージが来たらゲームループを抜ける
		if (winAPI.msg.message == WM_QUIT) {
			break;
		}
#pragma endregion

#pragma region DirectX毎フレーム処理ここから
		//-+-+-+-+-+-+-+-+-+-+-+-+-+-//
		// DirectX毎フレーム処理 ここから//
		//-+-+-+-+-+-+-+-+-+-+-+-+-+-//
		//キーボード情報の取得開始 //全キーの入力状態を取得する
		DirectXInput::InputUpdata();

		// バックバッファの番号を取得(2つなので0番か1番)
		UINT bbIndex = swapChain->GetCurrentBackBufferIndex();
		// 1.リソースバリアで書き込み可能に変更
		D3D12_RESOURCE_BARRIER barrierDesc{};
		barrierDesc.Transition.pResource = backBuffers[bbIndex]; // バックバッファを指定
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT; // 表示状態から
		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET; // 描画状態へ
		commandList->ResourceBarrier(1, &barrierDesc);

		// 2.描画先の変更
		// レンダーターゲットビューのハンドルを取得
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap->GetCPUDescriptorHandleForHeapStart();
		rtvHandle.ptr += bbIndex * device->GetDescriptorHandleIncrementSize(rtvHeapDesc.Type);
		//commandList->OMSetRenderTargets(1, &rtvHandle, false, nullptr);
		
		//深度ステンシルビュー用デスクリプタヒープのハンドルを取得
		D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvHeap->GetCPUDescriptorHandleForHeapStart();
		commandList->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);

		// 3.画面クリア R G B A
		FLOAT clearColor[] = { 0.1f,0.25f, 0.5f,0.0f }; // 青っぽい色

		//ーーーーーーーーここから更新処理記述ーーーーーーーーーー//
		////数字の0キーが押されていたら
		if (DirectXInput::IsKeyDown(DIK_SPACE)) {
			clearColor[0] = 0.8f;
		}

		if (DirectXInput::IsKeyTrigger(DIK_1)) {
			if (viewport.GetPiplineState() == D3D12_FILL_MODE_WIREFRAME) {
				viewport.SetPiplineState(D3D12_FILL_MODE_SOLID);
			}
			else if (viewport.GetPiplineState() == D3D12_FILL_MODE_SOLID) {
				viewport.SetPiplineState(D3D12_FILL_MODE_WIREFRAME);
			}
		}
		//塗りつぶし・ワイヤーフレーム更新
		viewport.PipelineStateUpdata(device);
		//背景色更新
		commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
		commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);;

		//カメラ移動

		if (DirectXInput::IsKeyDown(DIK_D)) {
			angle += XMConvertToRadians(1.0f);
		}
		if (DirectXInput::IsKeyDown(DIK_A)) {
			angle -= XMConvertToRadians(1.0f);
		}
		//angleラジアンだけY軸周りに回転。半径は-100
		eye.x = -20 * sinf(angle);
		eye.z = -20 * cosf(angle);
		matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
		//定数バッファに転送
		constMapTransform->mat = matView * matProjection;

		//ワールド座標

			//座標移動
		//if (DirectXInput::IsKeyDown(DIK_UP)) { position.z += 1.0f; }
		//else if (DirectXInput::IsKeyDown(DIK_DOWN)) { position.z -= 1.0f; }
		//if (DirectXInput::IsKeyDown(DIK_RIGHT)) { position.x += 1.0f; }
		//else if (DirectXInput::IsKeyDown(DIK_LEFT)) { position.x -= 1.0f; }

		//matScale = XMMatrixScaling(scale.x, scale.y, scale.z);

		//matRot = XMMatrixIdentity();
		//matRot *= XMMatrixRotationZ(rotation.z);
		//matRot *= XMMatrixRotationX(rotation.x);
		//matRot *= XMMatrixRotationY(rotation.y);

		//matTrans = XMMatrixTranslation(position.x, position.y, position.z);

		//matWorld = XMMatrixIdentity();	//変数をリセット
		//matWorld *= matScale;
		//matWorld *= matRot;
		//matWorld *= matTrans;	//ワールド行列に平行移動を反映

		////定数バッファに転送
		//constMapTransform->mat = matWorld * matView * matProjection;


		//三角形の色変更
		//testColor.x += 0.01f;
		////値を書き込むと自動的に転送される
		//constMapMaterial->colro = testColor;

		//ーーーーーーーーーーここまで更新処理記入ーーーーーーーーーー//
#pragma endregion

#pragma region 描画コマンド
		///////////////////////
		// 4.描画コマンドここから//
		///////////////////////

		//インデックスバッファビューの設定コマンド
		commandList->IASetIndexBuffer(&ibView);
		// シザー矩形
		D3D12_RECT scissorRect{};
		//指定座標の中のみ描画するための処理
		scissorRect.left = 0; // 切り抜き座標左
		scissorRect.right = scissorRect.left + winAPI.GetWinWidth(); // 切り抜き座標右
		scissorRect.top = 0; // 切り抜き座標上
		scissorRect.bottom = scissorRect.top + winAPI.GetWinHeight(); // 切り抜き座標下
		// シザー矩形設定コマンドを、コマンドリストに積む
		commandList->RSSetScissorRects(1, &scissorRect);
		//-------------------------左上
		// ビューポート設定コマンド
		viewport.SetViewport((float)winAPI.GetWinWidth(), (float)winAPI.GetWinHeight(), 0, 0, 0, 1.0f);
		// パイプラインステートとルートシグネチャの設定コマンド
		viewport.SetPipeline(commandList);
		// プリミティブ形状の設定コマンド
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		// D3D_PRIMITIVE_TOPOLOGY_POINTLIST		点のリスト
		// D3D_PRIMITIVE_TOPOLOGY_LINELIST		線のリスト
		// D3D_PRIMITIVE_TOPOLOGY_LINESTRIP		線のストリップ
		// D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST	三角形リスト
		// D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP	三角形のストリップ

		// 頂点バッファビューの設定コマンド
		commandList->IASetVertexBuffers(0, 1, &vbView);

		//定数バッファビュー
		commandList->SetGraphicsRootConstantBufferView(0, constBufferMaterial->GetGPUVirtualAddress());

		//SRVヒープの設定コマンド
		commandList->SetDescriptorHeaps(1, &srvHeap);
		//SRVヒープの先頭ハンドルを取得(SRVを指しているはず)
		D3D12_GPU_DESCRIPTOR_HANDLE srvGpuhHandle = srvHeap->GetGPUDescriptorHandleForHeapStart();
		//SRVヒープの先頭にあるSRVをルートパラメータ1番に設定
		commandList->SetGraphicsRootDescriptorTable(1, srvGpuhHandle);
		//定数バッファビュー（CBV）の設定コマンド
		commandList->SetGraphicsRootConstantBufferView(2, constBuffTransform->GetGPUVirtualAddress());

		// 描画コマンド
		commandList->DrawIndexedInstanced(_countof(indices), 1, 0, 0, 0); // 全ての頂点を使って描画
		////-------------------------右上
		//// ビューポート設定コマンド
		//Viewport::SetViewport((float)window_width / 2, (float)window_height / 2, (float)window_width / 2, (float)0);
		//// パイプラインステートとルートシグネチャの設定コマンド
		//Viewport::SetPipeline(commandList);
		//// 頂点バッファビューの設定コマンド
		//commandList->IASetVertexBuffers(0, 1, &vbView);
		//// 描画コマンド
		//commandList->DrawInstanced(_countof(vertices), 1, 0, 0); // 全ての頂点を使って描画
		////-------------------------左下
		//// ビューポート設定コマンド
		//Viewport::SetViewport((float)300, (float)window_height / 2, 0, 300, 0, 1.0f);
		//// パイプラインステートとルートシグネチャの設定コマンド
		//Viewport::SetPipeline(commandList);
		//// 頂点バッファビューの設定コマンド
		//commandList->IASetVertexBuffers(0, 1, &vbView);
		//// 描画コマンド
		//commandList->DrawInstanced(_countof(vertices), 1, 0, 0); // 全ての頂点を使って描画
		////-----------------------右下
		//// ビューポート設定コマンド
		//Viewport::SetViewport((float)300, (float)500, 750, 400, 0, 1.0f);
		//// パイプラインステートとルートシグネチャの設定コマンド
		//Viewport::SetPipeline(commandList);
		//// 頂点バッファビューの設定コマンド
		//commandList->IASetVertexBuffers(0, 1, &vbView);
		//// 描画コマンド
		//commandList->DrawInstanced(_countof(vertices), 1, 0, 0); // 全ての頂点を使って描画

		///////////////////////
		// 4.描画コマンドここまで//
		///////////////////////
#pragma endregion

#pragma region DirectX毎フレーム処理ここまで
			// 5.リソースバリアを戻す
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET; // 描画状態から
		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT; // 表示状態へ
		commandList->ResourceBarrier(1, &barrierDesc);

		// 命令のクローズ
		result = commandList->Close();
		assert(SUCCEEDED(result));
		// コマンドリストの実行
		ID3D12CommandList* commandLists[] = { commandList };
		commandQueue->ExecuteCommandLists(1, commandLists);
		// 画面に表示するバッファをフリップ(裏表の入替え)
		result = swapChain->Present(1, 0);
		assert(SUCCEEDED(result));

		// コマンドの実行完了を待つ
		commandQueue->Signal(fence, ++fenceVal);
		if (fence->GetCompletedValue() != fenceVal) {
			HANDLE event = CreateEvent(nullptr, false, false, nullptr);
			fence->SetEventOnCompletion(fenceVal, event);
			WaitForSingleObject(event, INFINITE);
			CloseHandle(event);
		}
		// キューをクリア
		result = commandAllocator->Reset();
		assert(SUCCEEDED(result));
		// 再びコマンドリストを貯める準備
		result = commandList->Reset(commandAllocator, nullptr);
		assert(SUCCEEDED(result));
		//-+-+-+-+-+-+-+-+-+-+-+-+-+-//
		// DirectX毎フレーム処理 ここまで//
		//-+-+-+-+-+-+-+-+-+-+-+-+-+-//
#pragma endregion
	}
	//元データ開放
	//delete[] imageData;

	//コンソールへの文字出力
	//OutputDebugStringA("Hello,DirectX!!\n");
	return 0;
}



