#pragma once
#include "WinAPI.h"
#include "GraphicsCmnd.h"
class DirextX
{
public:
	void DirectXIni();
	//�A�_�v�^�[�񋓁E�I��
	void AdapterIni();
	//�f�o�C�X�񋓁E�I��
	void DeviceIni();
	//�R�}���h���X�g����
	void CommandListIni();
	//�X���b�v�`�F�[���̐ݒ�
	void SwapChain();
	//�f�X�N���v�^�q�[�v
	void DescriptHeap();
	//�����_�[�^�[�Q�b�g�r���[
	void RenderTargetView();
	//�t�F���X
	void FenceIni();
	//�f�o�b�O���C���[
	void Debuglayer();

	//�O���t�B�b�N�X�R�}���h�X�V
	void UpdataGraficsCommand();
	//��ʃN���A�R�}���h
	//�o�b�N�o�b�t�@�Ɏc����2�t���[���O�̊G���w��E�œh��Ԃ�
	void ClearCanvas(FLOAT clearColor);
	//�O���t�B�b�N�X�R�}���h���s
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
	
	// �����ɓ���̖��O�����A�_�v�^�[�I�u�W�F�N�g������
	IDXGIAdapter4* tmpAdapter = nullptr;

	D3D_FEATURE_LEVEL featureLevel;

	std::vector<ID3D12Resource*> backBuffers;

	// �X���b�v�`�F�[���̐ݒ�
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	// �f�X�N���v�^�q�[�v�̐ݒ�
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};

	

	// �t�F���X�̐���
	ID3D12Fence* fence = nullptr;
	UINT64 fenceVal = 0;

	//�f�o�b�O���C���[
	ID3D12Debug* debugController;
	
};

