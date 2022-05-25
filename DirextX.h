#pragma once
#include "WinAPI.h"
#include "GraphicsCmnd.h"
class DirextX
{
public:
	void DirectXIni();
	//アダプター列挙・選別
	void AdapterIni();
	//デバイス列挙・選別
	void DeviceIni();
	//コマンドリスト生成
	void CommandListIni();
	//スワップチェーンの設定
	void SwapChain();
	//デスクリプタヒープ
	void DescriptHeap();
	//レンダーターゲットビュー
	void RenderTargetView();
	//フェンス
	void FenceIni();
	//デバッグレイヤー
	void Debuglayer();

	//グラフィックスコマンド更新
	void UpdataGraficsCommand();
	//画面クリアコマンド
	//バックバッファに残った2フレーム前の絵を指定職で塗りつぶす
	void ClearCanvas(FLOAT clearColor);
	//グラフィックスコマンド実行
	void ExecuteGraficsCommand();

private:
	WinAPI winAPI;
	GraphicsCmnd graCmd;

	ID3D12Device* device = nullptr;
	IDXGIFactory7* dxgiFactory = nullptr;
	IDXGISwapChain4* swapChain = nullptr;
	ID3D12CommandAllocator* commandAllocator = nullptr;
	ID3D12GraphicsCommandList* commandList = nullptr;
	ID3D12CommandQueue* commandQueue = nullptr;
	ID3D12DescriptorHeap* rtvHeap = nullptr;
	
	// ここに特定の名前を持つアダプターオブジェクトが入る
	IDXGIAdapter4* tmpAdapter = nullptr;

	D3D_FEATURE_LEVEL featureLevel;

	std::vector<ID3D12Resource*> backBuffers;

	// スワップチェーンの設定
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	// デスクリプタヒープの設定
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};

	

	// フェンスの生成
	ID3D12Fence* fence = nullptr;
	UINT64 fenceVal = 0;

	//デバッグレイヤー
	ID3D12Debug* debugController;
	
};

