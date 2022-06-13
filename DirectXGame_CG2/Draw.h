#pragma once
#include <vector>
#include <DirectXMath.h>
class Draw
{
public:
	void Ini(ID3D12Device* device);

	void ViewPortSet(float width, float height, float LeftX, float LeftY, ID3D12GraphicsCommandList* commandList);

	void ScissorSet(float left, float right, float top, float bottom, ID3D12GraphicsCommandList* commandList);

	void DrawShape(ID3D12GraphicsCommandList* commandList);

private:
	std::vector<XMFLOAT3> vertices;

	// 頂点データ全体のサイズ = 頂点データ一つ分のサイズ * 頂点データの要素数
	UINT sizeVB;

	D3D12_HEAP_PROPERTIES heapProp{}; // ヒープ設定
	// リソース設定
	D3D12_RESOURCE_DESC resDesc{};
	// 頂点バッファの生成
	ID3D12Resource* vertBuff = nullptr;
	// GPU上のバッファに対応した仮想メモリ(メインメモリ上)を取得
	XMFLOAT3* vertMap = nullptr;
	// 頂点バッファビューの作成
	D3D12_VERTEX_BUFFER_VIEW vbView{};

	ID3DBlob* vsBlob = nullptr; // 頂点シェーダオブジェクト
	ID3DBlob* psBlob = nullptr; // ピクセルシェーダオブジェクト
	ID3DBlob* errorBlob = nullptr; // エラーオブジェクト

	// 頂点レイアウト
	std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout;
	
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
	
};

