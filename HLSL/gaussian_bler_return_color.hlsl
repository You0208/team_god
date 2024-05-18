
// UNIT.32
#include "fullscreen_quad.hlsli"
#include "ConstantBuffer.hlsli"

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
#define LINEAR_BORDER_BLACK 3
#define LINEAR_BORDER_WHITE 4
SamplerState sampler_states[5] : register(s0);
Texture2D texture_maps[4] : register(t0);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = texture_maps[0].Sample(sampler_states[POINT], pin.texcoord);
    float4 bloom = texture_maps[1].Sample(sampler_states[POINT], pin.texcoord);
    
    float3 fragment_color = color.rgb + bloom.rgb;
    return float4(fragment_color, color.a);
}
