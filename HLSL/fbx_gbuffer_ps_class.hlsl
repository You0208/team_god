#include "light.hlsli"
#include "gbuffer.hlsli"


#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
// SHADOW
#define LINEAR_BORDER_BLACK 3
#define LINEAR_BORDER_WHITE 4

SamplerState sampler_states[5] : register(s0);
Texture2D texture_maps[4] : register(t0);
Texture2D metalSmoothTex : register(t4);

//モデル用のピクセルシェーダー
PSGBufferOut main(VS_OUT pin)
{
    GBufferData output;
    float4 albedoColor = texture_maps[0].Sample(sampler_states[ANISOTROPIC], pin.texcoord);
    output.base_color = albedoColor;
    
    // 従ベクトル
    float3 N = normalize(pin.world_normal.xyz);
    // 接ベクトル 
    float3 T = normalize(pin.world_tangent.xyz);
    float sigma = pin.world_tangent.w;
    
    T = normalize(T - N * dot(N, T));
    float3 B = normalize(cross(N, T) * sigma);

    float4 normal = texture_maps[1].Sample(sampler_states[LINEAR], pin.texcoord);
    normal = (normal * 2.0) - 1.0;
    N = normalize((normal.x * T) + (normal.y * B) + (normal.z * N));
    
    output.w_normal = float4(N, 0);
    
    output.w_position = pin.world_position;
    
    output.roughness = 1.0f - metalSmoothTex.Sample(sampler_states[ANISOTROPIC], pin.texcoord).a;
    output.metalness = metalSmoothTex.Sample(sampler_states[ANISOTROPIC], pin.texcoord).r;
    return EncodeGBuffer(output, view_projection);
}