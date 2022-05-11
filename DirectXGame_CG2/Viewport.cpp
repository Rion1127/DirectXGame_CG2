//#include "Viewport.h"
//#include <vector>
//#include <DirectXMath.h>
//#include <cassert>
//#include <d3dcommon.h>
//#include <d3d12.h>
//#pragma comment(lib, "d3dcompiler.lib")
//#pragma comment(lib, "d3d12.lib")
//#pragma comment(lib, "dxgi.lib")
//#include <combaseapi.h>

#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <iostream>
#include <cassert>
#include <vector>
#include <vector>
#include <string>
#include <DirectXMath.h>
#include "DirectXInput.h"
#include "Viewport.h"
using namespace DirectX;
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#pragma region
D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
	{
	"POSITION",									//�Z�}���e�B�b�N��
	0,											//�����Z�}���e�B�b�N������������Ƃ��Ɏg���C���f�b�N�X(�O�ł悢)
	DXGI_FORMAT_R32G32B32_FLOAT,				//�v�f���ƃr�b�g��������(XYZ�̂R��float�^�Ȃ̂�R32G32B32_FLOAT)
	0,											//���̓X���b�g�C���f�b�N�X(0�ł悢)
	D3D12_APPEND_ALIGNED_ELEMENT,				//�f�[�^�̃I�t�Z�b�g�l(D3D12_APPEND_ALIGNED_ELEMENT���Ǝ����ݒ�)
	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,	//���̓f�[�^���(�W����D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA)
	0											//��x�ɕ`�悷��C���X�^���X��(0�ł悢)
	}, // (1�s�ŏ������ق������₷��)
	//���W�ȊO�Ɂ@�F�A�e�N�X�`��UV����n���ꍇ�͂���ɑ�����
	{
		"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
	}
};

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
D3D12_ROOT_PARAMETER rootParam = {};

#pragma endregion

void Viewport::ViewPortIni(ID3DBlob* vsBlob, ID3DBlob* psBlob, ID3DBlob* errorBlob, HRESULT* result, ID3D12Device* device)
{
	// �V�F�[�_�[�̐ݒ�
	pipelineDesc.VS.pShaderBytecode = vsBlob->GetBufferPointer();
	pipelineDesc.VS.BytecodeLength = vsBlob->GetBufferSize();
	pipelineDesc.PS.pShaderBytecode = psBlob->GetBufferPointer();
	pipelineDesc.PS.BytecodeLength = psBlob->GetBufferSize();

	// �T���v���}�X�N�̐ݒ�
	pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // �W���ݒ�

	// ���X�^���C�U�̐ݒ�
	pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE; // �J�����O���Ȃ�
	pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID; // �|���S�����h��Ԃ�	SOLID = �h��Ԃ�  WIREFRAME = ���C���[�t���[��
	pipelineDesc.RasterizerState.DepthClipEnable = true; // �[�x�N���b�s���O��L����

	// �u�����h�X�e�[�g
	pipelineDesc.BlendState.RenderTarget[0].RenderTargetWriteMask
		= D3D12_COLOR_WRITE_ENABLE_ALL; // RBGA�S�Ẵ`�����l����`��
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = pipelineDesc.BlendState.RenderTarget[0];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

#pragma region �u�����h�ݒ�
	//���ʐݒ�i�A���t�@�l�j
	blenddesc.BlendEnable = true;					//�u�����h��L���ɂ���
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;	//���Z
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;		//�\�[�X�̒l��100%�g��
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;	//�f�X�g�̒l��  0%�g��

	////���Z����
	//blenddesc.BlendOp = D3D12_BLEND_OP_ADD;			//���Z
	//blenddesc.SrcBlend = D3D12_BLEND_ONE;			//�\�[�X�̒l��100%�g��
	//blenddesc.DestBlend = D3D12_BLEND_ONE;			//�f�X�g�̒l��100%�g��

	////���Z����
	//blenddesc.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;//�f�X�g����\�[�X�����Z
	//blenddesc.SrcBlend = D3D12_BLEND_ONE;			//�\�[�X�̒l��100%�g��
	//blenddesc.DestBlend = D3D12_BLEND_ONE;			//�f�X�g�̒l��100%�g��

	////�F���]
	//blenddesc.BlendOp = D3D12_BLEND_OP_ADD;			//���Z
	//blenddesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;//1.0f-�f�X�g�J���[�̒l
	//blenddesc.DestBlend = D3D12_BLEND_ZERO;			//�g��Ȃ�

	//����������
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;			//���Z
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;		//�\�[�X�̃A���t�@�l
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;//1.0f-�\�[�X�̃A���t�@�l
#pragma endregion

	// ���_���C�A�E�g�̐ݒ�
	pipelineDesc.InputLayout.pInputElementDescs = inputLayout;
	pipelineDesc.InputLayout.NumElements = _countof(inputLayout);

	// �}�`�̌`��ݒ�
	pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// ���̑��̐ݒ�
	pipelineDesc.NumRenderTargets = 1; // �`��Ώۂ�1��
	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0~255�w���RGBA
	pipelineDesc.SampleDesc.Count = 1; // 1�s�N�Z���ɂ�1��T���v�����O
	//���[�g�̃p�����[�^�ݒ�
	rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	//�萔�o�b�t�@�r���[
	rootParam.Descriptor.ShaderRegister = 0;					//�萔�o�b�t�@�ԍ�
	rootParam.Descriptor.RegisterSpace = 0;						//�f�t�H���g�l
	rootParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	//�S�ẴV�F�[�_���猩����
	//// ���[�g�V�O�l�`��
	//ID3D12RootSignature* rootSignature;
	//// ���[�g�V�O�l�`���̐ݒ�
	//D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = &rootParam;	
	rootSignatureDesc.NumParameters = 1;

	//// ���[�g�V�O�l�`���̃V���A���C�Y
	//ID3DBlob* rootSigBlob = nullptr;
	*result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob, &errorBlob);
	assert(SUCCEEDED(result));
	*result = device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(result));
	rootSigBlob->Release();
	// �p�C�v���C���Ƀ��[�g�V�O�l�`�����Z�b�g
	pipelineDesc.pRootSignature = rootSignature;

	//// �p�C�v�����X�e�[�g�̐���
	//ID3D12PipelineState* pipelineState = nullptr;
	*result = device->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&pipelineState));
	assert(SUCCEEDED(result));

	
}
//�ʂ�Ԃ�or���C���[�t���[���Z�b�g
void Viewport::SetPiplineState(D3D12_FILL_MODE mode)
{
	pipelineDesc.RasterizerState.FillMode = mode;
}
//�ʂ�Ԃ�or���C���[�t���[���X�V
void Viewport::PipelineStateUpdata(HRESULT* result, ID3D12Device* device)
{
	*result = device->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&pipelineState));
	assert(SUCCEEDED(result));
}
// �r���[�|�[�g�ݒ�R�}���h
void Viewport::SetViewport(float Width, float height, float topLeftX, float topLeftY, float minDepth, float maxDepth)
{
	viewport.Width = Width;
	viewport.Height = height;
	viewport.TopLeftX = topLeftX;				//��ʂ̂ǂ̍��W������W(0,0)�ɂ��邩�����߂�
	viewport.TopLeftY = topLeftY;
	viewport.MinDepth = minDepth;
	viewport.MaxDepth = maxDepth;
}
// �p�C�v���C���X�e�[�g�ƃ��[�g�V�O�l�`���̐ݒ�R�}���h
void Viewport::SetPipeline(ID3D12GraphicsCommandList* commandList)
{
	// �r���[�|�[�g�ݒ�R�}���h���A�R�}���h���X�g�ɐς�
	commandList->RSSetViewports(1, &viewport);
	// �p�C�v���C���X�e�[�g�ƃ��[�g�V�O�l�`���̐ݒ�R�}���h
	commandList->SetPipelineState(pipelineState);
	commandList->SetGraphicsRootSignature(rootSignature);
}

//void Viewport::Draw(ID3D12GraphicsCommandList* commandList,D3D12_VERTEX_BUFFER_VIEW vbView, const XMFLOAT3& vertices)
//{
//	std::vector<XMFLOAT3> a = { vertices };
//	std::size_t size_t = a.size();
//	
//
//	// ���_�o�b�t�@�r���[�̐ݒ�R�}���h
//	commandList->IASetVertexBuffers(0, 1, &vbView);
//
//	// �`��R�}���h
//	commandList->DrawInstanced(size_t, 1, 0, 0); // �S�Ă̒��_���g���ĕ`��
//
//}

int Viewport::GetPiplineState()
{
	return pipelineDesc.RasterizerState.FillMode;
}

