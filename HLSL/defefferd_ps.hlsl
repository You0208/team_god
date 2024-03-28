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
    // color
    float4 color = texture_maps[0].Sample(sampler_states[LINEAR_BORDER_BLACK], pin.texcoord);
    // Normal
    float3 normal = texture_maps[1].Sample(sampler_states[LINEAR_BORDER_BLACK], pin.texcoord).rgb;
    // position
    float3 position = texture_maps[2].Sample(sampler_states[LINEAR_BORDER_BLACK], pin.texcoord).rgb;
    float alpha = color.a;
    // 金属度
    float metallic = texture_maps[3].Sample(sampler_states[LINEAR_BORDER_BLACK], pin.texcoord).r;
    // 滑らかさ
    float smoothness = texture_maps[3].Sample(sampler_states[LINEAR_BORDER_BLACK], pin.texcoord).a;
#if 0
	// Gamma process
    const float GAMMA = 2.2f;
    color.rgb = pow(color.rgb, 1.0 / GAMMA);
#endif
    // 従ベクトル
    float3 N = normal.xyz;
  
    float3 L = normalize(-directional_light_direction.xyz);
    float3 diffuse = color.rgb * max(0, dot(N, L));
    float3 V = normalize(camera_position.xyz - position.xyz);
    float3 specular = pow(max(0, dot(N, normalize(V + L))), 128);
#if 1
	// Tone mapping : HDR -> SDR
    const float exposure = 1.2;
    color.rgb = 1 - exp(-color.rgb * exposure);
#endif


    return float4(diffuse + specular, alpha);
}
