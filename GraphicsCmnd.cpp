#include "GraphicsCmnd.h"
#include <vector>
#include <string>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
//���\�[�X�o���A�ύX�R�}���h
void GraphicsCmnd::ResourceBarrierOff(
	IDXGISwapChain4* swapChain,
	std::vector<ID3D12Resource*> backBuffers,
	ID3D12GraphicsCommandList* commandList)
{
	// �o�b�N�o�b�t�@�̔ԍ����擾(2�Ȃ̂�0�Ԃ�1��)
	bbIndex = swapChain->GetCurrentBackBufferIndex();
	// 1.���\�[�X�o���A�ŏ������݉\�ɕύX
	//D3D12_RESOURCE_BARRIER barrierDesc{};
	barrierDesc.Transition.pResource = backBuffers[bbIndex]; // �o�b�N�o�b�t�@���w��
	barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT; // �\����Ԃ���
	barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET; // �`���Ԃ�
	commandList->ResourceBarrier(1, &barrierDesc);
}
//�o�b�N�o�b�t�@��`���Ɏw�肷��
void GraphicsCmnd::ChangeCanvas(
	ID3D12DescriptorHeap* rtvHeap,
	ID3D12Device* device,
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc,
	ID3D12GraphicsCommandList* commandList)
{
	// 2.�`���̕ύX
		// �����_�[�^�[�Q�b�g�r���[�̃n���h�����擾
	rtvHandle = rtvHeap->GetCPUDescriptorHandleForHeapStart();
	rtvHandle.ptr += bbIndex * device->GetDescriptorHandleIncrementSize(rtvHeapDesc.Type);
	commandList->OMSetRenderTargets(1, &rtvHandle, false, nullptr);
}
//��ʃN���A�R�}���h
//�o�b�N�o�b�t�@�Ɏc����2�t���[���O�̊G���w��E�œh��Ԃ�
void GraphicsCmnd::ClearCanvas(ID3D12GraphicsCommandList* commandList,FLOAT clearColor)
{
	commandList->ClearRenderTargetView(rtvHandle, &clearColor, 0, nullptr);
}

void GraphicsCmnd::ResourceBarrierOn(ID3D12GraphicsCommandList* commandList)
{
	// 5.���\�[�X�o���A��߂�
	barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET; // �`���Ԃ���
	barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT; // �\����Ԃ�
	commandList->ResourceBarrier(1, &barrierDesc);
}

void GraphicsCmnd::CmndUpdata(
	ID3D12GraphicsCommandList* commandList,
	ID3D12CommandQueue* commandQueue,
	IDXGISwapChain4* swapChain)
{
	HRESULT result;
	// ���߂̃N���[�Y
	result = commandList->Close();
	assert(SUCCEEDED(result));
	// �R�}���h���X�g�̎��s
	ID3D12CommandList* commandLists[] = { commandList };
	commandQueue->ExecuteCommandLists(1, commandLists);
	// ��ʂɕ\������o�b�t�@���t���b�v(���\�̓��ւ�)
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
	// �R�}���h�̎��s������҂�
	commandQueue->Signal(fence, ++fenceVal);
	if (fence->GetCompletedValue() != fenceVal) {
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		fence->SetEventOnCompletion(fenceVal, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}
	// �L���[���N���A
	result = commandAllocator->Reset();
	assert(SUCCEEDED(result));
	// �ĂуR�}���h���X�g�𒙂߂鏀��
	result = commandList->Reset(commandAllocator, nullptr);
	assert(SUCCEEDED(result));
}
