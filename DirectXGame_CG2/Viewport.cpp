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
	"POSITION",									//セマンティック名
	0,											//同じセマンティック名が複数あるときに使うインデックス(０でよい)
	DXGI_FORMAT_R32G32B32_FLOAT,				//要素数とビット数を現す(XYZの３つでfloat型なのでR32G32B32_FLOAT)
	0,											//入力スロットインデックス(0でよい)
	D3D12_APPEND_ALIGNED_ELEMENT,				//データのオフセット値(D3D12_APPEND_ALIGNED_ELEMENTだと自動設定)
	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,	//入力データ種別(標準はD3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA)
	0											//一度に描画するインスタンス数(0でよい)
	}, // (1行で書いたほうが見やすい)
	//座標以外に　色、テクスチャUV等を渡す場合はさらに続ける
	{
		"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
	}
};

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
D3D12_ROOT_PARAMETER rootParam = {};

#pragma endregion

void Viewport::ViewPortIni(ID3DBlob* vsBlob, ID3DBlob* psBlob, ID3DBlob* errorBlob, HRESULT* result, ID3D12Device* device)
{
	// シェーダーの設定
	pipelineDesc.VS.pShaderBytecode = vsBlob->GetBufferPointer();
	pipelineDesc.VS.BytecodeLength = vsBlob->GetBufferSize();
	pipelineDesc.PS.pShaderBytecode = psBlob->GetBufferPointer();
	pipelineDesc.PS.BytecodeLength = psBlob->GetBufferSize();

	// サンプルマスクの設定
	pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定

	// ラスタライザの設定
	pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE; // カリングしない
	pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID; // ポリゴン内塗りつぶし	SOLID = 塗りつぶし  WIREFRAME = ワイヤーフレーム
	pipelineDesc.RasterizerState.DepthClipEnable = true; // 深度クリッピングを有効に

	// ブレンドステート
	pipelineDesc.BlendState.RenderTarget[0].RenderTargetWriteMask
		= D3D12_COLOR_WRITE_ENABLE_ALL; // RBGA全てのチャンネルを描画
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = pipelineDesc.BlendState.RenderTarget[0];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

#pragma region ブレンド設定
	//共通設定（アルファ値）
	blenddesc.BlendEnable = true;					//ブレンドを有効にする
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;	//加算
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;		//ソースの値を100%使う
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;	//デストの値を  0%使う

	////加算合成
	//blenddesc.BlendOp = D3D12_BLEND_OP_ADD;			//加算
	//blenddesc.SrcBlend = D3D12_BLEND_ONE;			//ソースの値を100%使う
	//blenddesc.DestBlend = D3D12_BLEND_ONE;			//デストの値を100%使う

	////減算合成
	//blenddesc.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;//デストからソースを減算
	//blenddesc.SrcBlend = D3D12_BLEND_ONE;			//ソースの値を100%使う
	//blenddesc.DestBlend = D3D12_BLEND_ONE;			//デストの値を100%使う

	////色反転
	//blenddesc.BlendOp = D3D12_BLEND_OP_ADD;			//加算
	//blenddesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;//1.0f-デストカラーの値
	//blenddesc.DestBlend = D3D12_BLEND_ZERO;			//使わない

	//半透明合成
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;			//加算
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;		//ソースのアルファ値
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;//1.0f-ソースのアルファ値
#pragma endregion

	// 頂点レイアウトの設定
	pipelineDesc.InputLayout.pInputElementDescs = inputLayout;
	pipelineDesc.InputLayout.NumElements = _countof(inputLayout);

	// 図形の形状設定
	pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// その他の設定
	pipelineDesc.NumRenderTargets = 1; // 描画対象は1つ
	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0~255指定のRGBA
	pipelineDesc.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング
	//ルートのパラメータ設定
	rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	//定数バッファビュー
	rootParam.Descriptor.ShaderRegister = 0;					//定数バッファ番号
	rootParam.Descriptor.RegisterSpace = 0;						//デフォルト値
	rootParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	//全てのシェーダから見える
	//// ルートシグネチャ
	//ID3D12RootSignature* rootSignature;
	//// ルートシグネチャの設定
	//D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = &rootParam;	
	rootSignatureDesc.NumParameters = 1;

	//// ルートシグネチャのシリアライズ
	//ID3DBlob* rootSigBlob = nullptr;
	*result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob, &errorBlob);
	assert(SUCCEEDED(result));
	*result = device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(result));
	rootSigBlob->Release();
	// パイプラインにルートシグネチャをセット
	pipelineDesc.pRootSignature = rootSignature;

	//// パイプランステートの生成
	//ID3D12PipelineState* pipelineState = nullptr;
	*result = device->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&pipelineState));
	assert(SUCCEEDED(result));

	
}
//ぬりつぶしorワイヤーフレームセット
void Viewport::SetPiplineState(D3D12_FILL_MODE mode)
{
	pipelineDesc.RasterizerState.FillMode = mode;
}
//ぬりつぶしorワイヤーフレーム更新
void Viewport::PipelineStateUpdata(HRESULT* result, ID3D12Device* device)
{
	*result = device->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&pipelineState));
	assert(SUCCEEDED(result));
}
// ビューポート設定コマンド
void Viewport::SetViewport(float Width, float height, float topLeftX, float topLeftY, float minDepth, float maxDepth)
{
	viewport.Width = Width;
	viewport.Height = height;
	viewport.TopLeftX = topLeftX;				//画面のどの座標から座標(0,0)にするかを決める
	viewport.TopLeftY = topLeftY;
	viewport.MinDepth = minDepth;
	viewport.MaxDepth = maxDepth;
}
// パイプラインステートとルートシグネチャの設定コマンド
void Viewport::SetPipeline(ID3D12GraphicsCommandList* commandList)
{
	// ビューポート設定コマンドを、コマンドリストに積む
	commandList->RSSetViewports(1, &viewport);
	// パイプラインステートとルートシグネチャの設定コマンド
	commandList->SetPipelineState(pipelineState);
	commandList->SetGraphicsRootSignature(rootSignature);
}

//void Viewport::Draw(ID3D12GraphicsCommandList* commandList,D3D12_VERTEX_BUFFER_VIEW vbView, const XMFLOAT3& vertices)
//{
//	std::vector<XMFLOAT3> a = { vertices };
//	std::size_t size_t = a.size();
//	
//
//	// 頂点バッファビューの設定コマンド
//	commandList->IASetVertexBuffers(0, 1, &vbView);
//
//	// 描画コマンド
//	commandList->DrawInstanced(size_t, 1, 0, 0); // 全ての頂点を使って描画
//
//}

int Viewport::GetPiplineState()
{
	return pipelineDesc.RasterizerState.FillMode;
}

