#pragma once
class GraphicsCmnd
{
public:

	void ResourceBarrierOff(
		IDXGISwapChain4* swapChain,
		std::vector<ID3D12Resource*> backBuffers,
		ID3D12GraphicsCommandList* commandList);

	void ChangeCanvas(ID3D12DescriptorHeap* rtvHeap,
		ID3D12Device* device,
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc,
		ID3D12GraphicsCommandList* commandList);

	void ClearCanvas(ID3D12GraphicsCommandList* commandList, FLOAT clearColor);

	void ResourceBarrierOn(ID3D12GraphicsCommandList* commandList);

	void CmndUpdata(ID3D12GraphicsCommandList* commandList,
		ID3D12CommandQueue* commandQueue,
		IDXGISwapChain4* swapChain);

	void UpdataWait(ID3D12CommandQueue* commandQueue,
		ID3D12Fence* fence,
		UINT64 fenceVal,
		ID3D12CommandAllocator* commandAllocator,
		ID3D12GraphicsCommandList* commandList);

private:
	UINT bbIndex;
	// 1.リソースバリアで書き込み可能に変更
	D3D12_RESOURCE_BARRIER barrierDesc{};
	// レンダーターゲットビューのハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;

};

