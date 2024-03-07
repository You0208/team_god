#include "skinned_mesh.hlsli"

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
// SHADOW
#define LINEAR_BORDER_BLACK 3
#define LINEAR_BORDER_WHITE 4

SamplerState sampler_states[5] : register(s0);
Texture2D texture_maps[4] : register(t0);

// SHADOW
SamplerComparisonState comparison_sampler_state : register(s5);
Texture2D shadow_map : register(t8);

float4 main(VS_OUT pin) : SV_TARGET
{
    // ダミーテクスチャ
    float4 color = texture_maps[0].Sample(sampler_states[ANISOTROPIC], pin.texcoord);
    float alpha = color.a;
#if 0
    // Inverse gamma process
    const float GAMMA = 2.2;
    color.rgb = pow(color.rgb, GAMMA);
#endif
    // 従ベクトル
    float3 N = normalize(pin.world_normal.xyz);
#if 1
    // 接ベクトル 
    float3 T = normalize(pin.world_tangent.xyz);
    float sigma = pin.world_tangent.w;
    
    T = normalize(T - N * dot(N, T));
    float3 B = normalize(cross(N, T) * sigma);

    float4 normal = texture_maps[1].Sample(sampler_states[LINEAR], pin.texcoord);
    normal = (normal * 2.0) - 1.0;
    N = normalize((normal.x * T) + (normal.y * B) + (normal.z * N));
#endif
	
    float3 L = normalize(-directional_light_direction.xyz);
    float3 diffuse = color.rgb * max(0, dot(N, L));
    float3 V = normalize(camera_position.xyz - pin.world_position.xyz);
    float3 specular = pow(max(0, dot(N, normalize(V + L))), 128);

	// SHADOW
#if 0
    const float shadow_depth_bias = 0.001;
#else
	// Here we have a maximum bias of 0.01 and a minimum of 0.001 based on the surface's normal and light direction. 
	const float shadow_depth_bias = max(0.01 * (1.0 - dot(N, L)), 0.001);
#endif

    float4 light_view_position = mul(pin.world_position, light_view_projection);
    light_view_position = light_view_position / light_view_position.w;
    float2 light_view_texcoord = 0;
    light_view_texcoord.x = light_view_position.x * +0.5 + 0.5;
    light_view_texcoord.y = light_view_position.y * -0.5 + 0.5;
    float depth = saturate(light_view_position.z - shadow_depth_bias);

    float3 shadow_factor = 1.0f;
    shadow_factor = shadow_map.SampleCmpLevelZero(comparison_sampler_state, light_view_texcoord, depth).xxx;

    //return float4(color.xyz, parameters.x);
    return float4((diffuse + specular) * shadow_factor /*SHADOW*/, parameters.x) * pin.color;
}