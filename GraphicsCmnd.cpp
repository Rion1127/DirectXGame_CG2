#include "GraphicsCmnd.h"
#include <vector>
#include <string>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
//リソースバリア変更コマンド
void GraphicsCmnd::ResourceBarrierOff(
	IDXGISwapChain4* swapChain,
	std::vector<ID3D12Resource*> backBuffers,
	ID3D12GraphicsCommandList* commandList)
{
	// バックバッファの番号を取得(2つなので0番か1番)
	bbIndex = swapChain->GetCurrentBackBufferIndex();
	// 1.リソースバリアで書き込み可能に変更
	//D3D12_RESOURCE_BARRIER barrierDesc{};
	barrierDesc.Transition.pResource = backBuffers[bbIndex]; // バックバッファを指定
	barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT; // 表示状態から
	barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET; // 描画状態へ
	commandList->ResourceBarrier(1, &barrierDesc);
}
//バックバッファを描画先に指定する
void GraphicsCmnd::ChangeCanvas(
	ID3D12DescriptorHeap* rtvHeap,
	ID3D12Device* device,
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc,
	ID3D12GraphicsCommandList* commandList)
{
	// 2.描画先の変更
		// レンダーターゲットビューのハンドルを取得
	rtvHandle = rtvHeap->GetCPUDescriptorHandleForHeapStart();
	rtvHandle.ptr += bbIndex * device->GetDescriptorHandleIncrementSize(rtvHeapDesc.Type);
	commandList->OMSetRenderTargets(1, &rtvHandle, false, nullptr);
}
//画面クリアコマンド
//バックバッファに残った2フレーム前の絵を指定職で塗りつぶす
void GraphicsCmnd::ClearCanvas(ID3D12GraphicsCommandList* commandList,FLOAT clearColor)
{
	commandList->ClearRenderTargetView(rtvHandle, &clearColor, 0, nullptr);
}

void GraphicsCmnd::ResourceBarrierOn(ID3D12GraphicsCommandList* commandList)
{
	// 5.リソースバリアを戻す
	barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET; // 描画状態から
	barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT; // 表示状態へ
	commandList->ResourceBarrier(1, &barrierDesc);
}

void GraphicsCmnd::CmndUpdata(
	ID3D12GraphicsCommandList* commandList,
	ID3D12CommandQueue* commandQueue,
	IDXGISwapChain4* swapChain)
{
	HRESULT result;
	// 命令のクローズ
	result = commandList->Close();
	assert(SUCCEEDED(result));
	// コマンドリストの実行
	ID3D12CommandList* commandLists[] = { commandList };
	commandQueue->ExecuteCommandLists(1, commandLists);
	// 画面に表示するバッファをフリップ(裏表の入替え)
	result = swapChain->Present(1, 0);
	assert(SUCCEEDED(result));
}

void GraphicsCmnd::UpdataWait(
	ID3D12CommandQueue* commandQueue,
	ID3D12Fence* fence,
	UINT64 fenceVal,
	ID3D12CommandAllocator* commandAllocator,
	ID3D12GraphicsCommandList* commandList)
{
	HRESULT result;
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
}
