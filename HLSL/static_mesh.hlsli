#if 0
struct VS_OUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	// UNIT.14
	float2 texcoord : TEXCOORD;
};
#else
struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 world_position : POSITION;
    float4 world_normal : NORMAL;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
};
#endif
