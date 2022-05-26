#pragma once
#include <d3d12.h>
#pragma comment(lib, "d3dcompiler.lib")

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
class Viewport
{
public:
	Viewport();

	void ViewPortIni(ID3DBlob* vsBlob, ID3DBlob* psBlob, ID3DBlob* errorBlob, ID3D12Device* device);
	//D3D12_FILL_MODE_WIREFRAME = ���C���[�t���[��
	//D3D12_FILL_MODE_SOLID = �h��Ԃ�
	void SetPiplineState(D3D12_FILL_MODE mode);
	//�ʂ�Ԃ�or���C���[�t���[���X�V
	void PipelineStateUpdata(ID3D12Device* device);
	// �r���[�|�[�g�ݒ�R�}���h
	void SetViewport(float Width, float height, float topLeftX, float topLeftY, float minDepth = 0, float maxDepth = 1.0f);
	// �p�C�v���C���X�e�[�g�ƃ��[�g�V�O�l�`���̐ݒ�R�}���h
	void SetPipeline(ID3D12GraphicsCommandList* commandList);
	//�`��R�}���h
	//static void Draw(ID3D12GraphicsCommandList* commandList, D3D12_VERTEX_BUFFER_VIEW vbView, const XMFLOAT3& vertices);

	int GetPiplineState();
private:
	// �O���t�B�b�N�X�p�C�v���C���ݒ�
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};
	// ���[�g�V�O�l�`��
	ID3D12RootSignature* rootSignature;
	// ���[�g�V�O�l�`���̐ݒ�
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	// ���[�g�V�O�l�`���̃V���A���C�Y
	ID3DBlob* rootSigBlob = nullptr;
	// �p�C�v�����X�e�[�g�̐���
	ID3D12PipelineState* pipelineState = nullptr;
	// �r���[�|�[�g�ݒ�R�}���h
	D3D12_VIEWPORT viewport{};
	//���[�g�̃p�����[�^�ݒ�
	D3D12_ROOT_PARAMETER rootParams[3] = {};
	//�f�X�N���v�^�����W�̐ݒ�
	D3D12_DESCRIPTOR_RANGE descriptorRange{};
	//�e�N�X�`���T���v���[�̐ݒ�
	D3D12_STATIC_SAMPLER_DESC samplerDesc{};
};

