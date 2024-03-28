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
   float4 color = texture_maps[0].Sample(sampler_states[LINEAR_BORDER_BLACK], pin.texcoord);
    float alpha = color.a;

#if 0
	// Gamma process
    const float GAMMA = 2.2f;
    color.rgb = pow(color.rgb, 1.0 / GAMMA);
#endif

    //return float4(color.rgb, alpha);
    
  
	// BLOOM
    float3 bloom_color = texture_maps[1].Sample(sampler_states[LINEAR_BORDER_BLACK], pin.texcoord).rgb;
    color.rgb += bloom_color;
	// FOG
    float3 fog_color = texture_maps[2].Sample(sampler_states[LINEAR_BORDER_BLACK], pin.texcoord).rgb;
    color.rgb += fog_color;
    
#if 1
	// Tone mapping : HDR -> SDR
    const float exposure = 1.2;
    color.rgb = 1 - exp(-color.rgb * exposure);
#endif


    return float4(color.rgb, alpha);
}
