//マテリアル
cbuffer ConstBufferDataMaterial : register(b0) {
	float4 color;	//色(RGBA)
}

//頂点シェーダの出力構造体
//(頂点シェーダからピクセルシェーダへのやり取りに使用する
struct VSOutput {
	
	float4 svpos : SV_POSITION;//システム用頂点座標
	float3 normal : NORMAL;//法線ベクトル
	float2 uv : TEXCOORD;//uv値
};
//3D変換強烈
cbuffer ConstBufferDataTransform : register(b1) {
	matrix mat;
}