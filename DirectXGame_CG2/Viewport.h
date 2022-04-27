#pragma once
#include <d3d12.h>
#pragma comment(lib, "d3dcompiler.lib")

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
class Viewport
{
public:
	static void ViewPortIni(ID3DBlob* vsBlob, ID3DBlob* psBlob, ID3DBlob* errorBlob, HRESULT* result, ID3D12Device* device);
	//D3D12_FILL_MODE_WIREFRAME = ワイヤーフレーム
	//D3D12_FILL_MODE_SOLID = 塗りつぶし
	static void SetPiplineState(D3D12_FILL_MODE mode);
	//ぬりつぶしorワイヤーフレーム更新
	static void PipelineStateUpdata(HRESULT* result, ID3D12Device* device);
	// ビューポート設定コマンド
	static void SetViewport(float Width, float height, float topLeftX, float topLeftY, float minDepth = 0, float maxDepth = 1.0f);
	// パイプラインステートとルートシグネチャの設定コマンド
	static void SetPipeline(ID3D12GraphicsCommandList* commandList);
	//描画コマンド
	static void Draw(ID3D12GraphicsCommandList* commandList, D3D12_VERTEX_BUFFER_VIEW vbView, const XMFLOAT3& vertices);

	static int GetPiplineState();
private:
	


};

