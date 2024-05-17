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


float Epsilon = 1e-10;
 
float3 RGBtoHCV(in float3 RGB)
{
    // Based on work by Sam Hocevar and Emil Persson
    float4 P = (RGB.g < RGB.b) ? float4(RGB.bg, -1.0, 2.0 / 3.0) : float4(RGB.gb, 0.0, -1.0 / 3.0);
    float4 Q = (RGB.r < P.x) ? float4(P.xyw, RGB.r) : float4(RGB.r, P.yzx);
    float C = Q.x - min(Q.w, Q.y);
    float H = abs((Q.w - Q.y) / (6 * C + Epsilon) + Q.z);
    return float3(H, C, Q.x);
}
float3 RGBtoHSV(in float3 RGB)
{
    float3 HCV = RGBtoHCV(RGB);
    float S = HCV.y / (HCV.z + Epsilon);
    return float3(HCV.x, S, HCV.z);
}
float3 HUEtoRGB(in float H)
{
    float R = abs(H * 6 - 3) - 1;
    float G = 2 - abs(H * 6 - 2);
    float B = 2 - abs(H * 6 - 4);
    return saturate(float3(R, G, B));
}
float3 HSVtoRGB(in float3 HSV)
{
    float3 RGB = HUEtoRGB(HSV.x);
    return ((RGB - 1) * HSV.y + 1) * HSV.z;
}

float3 reinhard_tone_mapping(float3 color)
{
    float luma = dot(color, float3(0.2126, 0.7152, 0.0722));
    float tone_mapped_luma = luma / (1. + luma);
    color *= tone_mapped_luma / luma;
    return color;
}
float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = texture_maps[0].Sample(sampler_states[LINEAR_BORDER_BLACK], pin.texcoord);
    float alpha = color.a;

#if 0
	// Gamma process
    const float GAMMA = 2.2f;
    color.rgb = pow(color.rgb, 1.0 / GAMMA);
#endif   
  
	// BLOOM
    float3 bloom_color = texture_maps[1].Sample(sampler_states[LINEAR_BORDER_BLACK], pin.texcoord).rgb;
    color.rgb += bloom_color;
    
    
    if (color.a <= 0.0f)
    {
        float3 tex_color = texture_maps[3].Sample(sampler_states[LINEAR_BORDER_BLACK], pin.texcoord).rgb;
        color.rgb += tex_color;
    }
    float3 fragment_color = color.rgb + bloom_color.rgb;


	// Tone map
    //fragment_color = reinhard_tone_mapping(fragment_color);

   // return float4(bloom_color, 1);
    

	// Gamma correction
    //const float INV_GAMMA = 1.0 / 2.2;
    //fragment_color = pow(fragment_color, INV_GAMMA);
    
    return float4(fragment_color, 1);

 //   float4 color = texture_maps[0].Sample(sampler_states[POINT], pin.texcoord);
 //   float4 bloom = texture_maps[1].Sample(sampler_states[POINT], pin.texcoord);
 //   ////color.rgb += tex_color;
 //   //return float4(tex_color, 1);
 //   //return float4(color.a, 0, 0, 1);
 //   if (color.a <= 0.0f)
 //   {
 //       float3 tex_color = texture_maps[3].Sample(sampler_states[LINEAR_BORDER_BLACK], pin.texcoord).rgb;
 //       color.rgb += tex_color;
 //       //return float4(tex_color, 1);
 //   }
    
 //   //// FOG
 //   //float3 fog_color = texture_maps[2].Sample(sampler_states[POINT], pin.texcoord).rgb;
    
 //   float3 fragment_color = color.rgb + bloom.rgb;
 //   float alpha = color.a;
 //  // return float4(fragment_color, 1);
	//// Tone map
 //   fragment_color = reinhard_tone_mapping(fragment_color);
    
	//// Gamma correction
 //   const float INV_GAMMA = 1.0 / 2.2;
 //   fragment_color = pow(fragment_color, INV_GAMMA);
 //   return float4(fragment_color, alpha);
}
