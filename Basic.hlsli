//�}�e���A��
cbuffer ConstBufferDataMaterial : register(b0) {
	float4 color;	//�F(RGBA)
}

//���_�V�F�[�_�̏o�͍\����
//(���_�V�F�[�_����s�N�Z���V�F�[�_�ւ̂����Ɏg�p����
struct VSOutput {
	
	float4 svpos : SV_POSITION;//�V�X�e���p���_���W
	float3 normal : NORMAL;//�@���x�N�g��
	float2 uv : TEXCOORD;//uv�l
};
//3D�ϊ�����
cbuffer ConstBufferDataTransform : register(b1) {
	matrix mat;
}