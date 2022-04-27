#pragma once
#include <d3d12.h>
#pragma comment(lib, "d3dcompiler.lib")

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
class Viewport
{
public:
	static void ViewPortIni(ID3DBlob* vsBlob, ID3DBlob* psBlob, ID3DBlob* errorBlob, HRESULT* result, ID3D12Device* device);
	//D3D12_FILL_MODE_WIREFRAME = ���C���[�t���[��
	//D3D12_FILL_MODE_SOLID = �h��Ԃ�
	static void SetPiplineState(D3D12_FILL_MODE mode);
	//�ʂ�Ԃ�or���C���[�t���[���X�V
	static void PipelineStateUpdata(HRESULT* result, ID3D12Device* device);
	// �r���[�|�[�g�ݒ�R�}���h
	static void SetViewport(float Width, float height, float topLeftX, float topLeftY, float minDepth = 0, float maxDepth = 1.0f);
	// �p�C�v���C���X�e�[�g�ƃ��[�g�V�O�l�`���̐ݒ�R�}���h
	static void SetPipeline(ID3D12GraphicsCommandList* commandList);
	//�`��R�}���h
	static void Draw(ID3D12GraphicsCommandList* commandList, D3D12_VERTEX_BUFFER_VIEW vbView, const XMFLOAT3& vertices);

	static int GetPiplineState();
private:
	


};

