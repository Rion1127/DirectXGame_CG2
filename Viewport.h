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
	//D3D12_FILL_MODE_WIREFRAME = ワイヤーフレーム
	//D3D12_FILL_MODE_SOLID = 塗りつぶし
	void SetPiplineState(D3D12_FILL_MODE mode);
	//ぬりつぶしorワイヤーフレーム更新
	void PipelineStateUpdata(ID3D12Device* device);
	// ビューポート設定コマンド
	void SetViewport(float Width, float height, float topLeftX, float topLeftY, float minDepth = 0, float maxDepth = 1.0f);
	// パイプラインステートとルートシグネチャの設定コマンド
	void SetPipeline(ID3D12GraphicsCommandList* commandList);
	//描画コマンド
	//static void Draw(ID3D12GraphicsCommandList* commandList, D3D12_VERTEX_BUFFER_VIEW vbView, const XMFLOAT3& vertices);

	int GetPiplineState();
private:
	// グラフィックスパイプライン設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};
	// ルートシグネチャ
	ID3D12RootSignature* rootSignature;
	// ルートシグネチャの設定
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	// ルートシグネチャのシリアライズ
	ID3DBlob* rootSigBlob = nullptr;
	// パイプランステートの生成
	ID3D12PipelineState* pipelineState = nullptr;
	// ビューポート設定コマンド
	D3D12_VIEWPORT viewport{};
	//ルートのパラメータ設定
	D3D12_ROOT_PARAMETER rootParams[3] = {};
	//デスクリプタレンジの設定
	D3D12_DESCRIPTOR_RANGE descriptorRange{};
	//テクスチャサンプラーの設定
	D3D12_STATIC_SAMPLER_DESC samplerDesc{};
};

