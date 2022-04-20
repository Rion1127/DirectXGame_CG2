
float4 main(float4 pos : POSITION) : SV_POSITION
{
	float4 pos2 = pos;				//コピー
	const float PI = 3.14159;		//π
	float angle = PI * 0.50f;		//回転角(ラジアン)
	//pos2.x = pos.x * cos(angle) - pos.y * sin(angle);
	//pos2.y = pos.x * sin(angle) + pos.y * cos(angle);

	return pos2 ;
}