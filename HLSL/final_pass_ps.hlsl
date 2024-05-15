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

float3 reinhard_tone_mapping(float3 color)
{
    float luma = dot(color, float3(0.2126, 0.7152, 0.0722));
    float tone_mapped_luma = luma / (1. + luma);
    color *= tone_mapped_luma / luma;
    return color;
}
float4 main(VS_OUT pin) : SV_TARGET
{
//    float4 color = texture_maps[0].Sample(sampler_states[LINEAR_BORDER_BLACK], pin.texcoord);
//    float alpha = color.a;

//#if 0
//	// Gamma process
//    const float GAMMA = 2.2f;
//    color.rgb = pow(color.rgb, 1.0 / GAMMA);
//#endif   
  
//	// BLOOM
//    float3 bloom_color = texture_maps[1].Sample(sampler_states[LINEAR_BORDER_BLACK], pin.texcoord).rgb;
//    color.rgb += bloom_color;
    
//    float3 fragment_color = color.rgb + bloom_color.rgb;


//	// Tone map
//    fragment_color = reinhard_tone_mapping(fragment_color);

//   // return float4(bloom_color, 1);
    
//    if (color.a <= 0.0f)
//    {
//        float3 tex_color = texture_maps[3].Sample(sampler_states[LINEAR_BORDER_BLACK], pin.texcoord).rgb;
//        color.rgb += tex_color;
//    }
    

//	// Gamma correction
//    const float INV_GAMMA = 1.0 / 2.2;
//    fragment_color = pow(fragment_color, INV_GAMMA);


//    return float4(fragment_color, alpha);

    float4 color = texture_maps[0].Sample(sampler_states[POINT], pin.texcoord);
    float4 bloom = texture_maps[1].Sample(sampler_states[POINT], pin.texcoord);
    // FOG
    float3 fog_color = texture_maps[2].Sample(sampler_states[POINT], pin.texcoord).rgb;
    
    float3 fragment_color = color.rgb + bloom.rgb + fog_color;
    float alpha = color.a;
    //return float4(fragment_color, alpha);

	// Tone map
    fragment_color = reinhard_tone_mapping(fragment_color);
    
	// Gamma correction
    const float INV_GAMMA = 1.0 / 2.2;
    fragment_color = pow(fragment_color, INV_GAMMA);

    return float4(fragment_color, alpha);
}
