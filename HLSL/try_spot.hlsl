#include "lim_try.hlsli"

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
#define LINEAR_BORDER_BLACK 3
#define LINEAR_BORDER_WHITE 4

SamplerState sampler_states[3] : register(s0);
Texture2D texture_maps[8] : register(t0);

Texture2D noise_map : register(t9); // 先生ノイズは3D

Texture2D roughness_ : register(t4);
Texture2D metalness_ : register(t5);

// SHADOW
SamplerComparisonState comparison_sampler_state : register(s5);
Texture2D shadow_map : register(t8);

///////////////////////////////////////////
// シェーダーリソース
///////////////////////////////////////////
// モデルテクスチャ

///////////////////////////////////////////
// サンプラーステート
///////////////////////////////////////////
sampler g_sampler : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 diffuse_color = texture_maps[0].Sample(sampler_states[ANISOTROPIC], pin.texcoord);
    float3 E = normalize(pin.world_position.xyz - camera_position.xyz);
    float3 L = normalize(directional_light_direction.xyz);
    float3x3 mat = { normalize(pin.world_tangent.xyz), normalize(pin.binormal), normalize(pin.world_normal.xyz) };
    float3 N = texture_maps[1].Sample(sampler_states[ANISOTROPIC], pin.texcoord).rgb;
    // ノーマルテクスチャ法線をワールドへ変換
    N = normalize(mul(N * 2.0f - 1.0f, mat));
    float3 ambient = ambient_color.rgb * ambient_color.rgb;
    
    float3 directional_diffuse = CalcLambert(N, L, directional_light_color.rgb, float3(0.8f,0.8f,0.8f));
    float3 directional_specular = CalcPhongSpecular(N, L, E, directional_light_color.rgb, float3(1.0f, 1.0f, 1.0f));
// 点光源の処理
    float3 point_diffuse = 0;
    float3 point_specular = 0;
    for (int i = 0; i < 8; ++i)
    {
        float3 LP = pin.world_position.xyz - point_light[i].position.xyz;
        float len = length(LP);
        if (len >= point_light[i].range)
            continue;
        float attenuateLength = saturate(1.0f - len / point_light[i].range);
        float attenuation = attenuateLength * attenuateLength;
        LP /= len;
        point_diffuse += CalcLambert(N, LP, point_light[i].color.rgb, float3(0.8f, 0.8f, 0.8f)) * attenuation;
        point_specular += CalcPhongSpecular(N, LP, E, point_light[i].color.rgb, float3(1.0f, 1.0f, 1.0f)) * attenuation;
    }
// スポットライトの処理
    float3 spot_diffuse = 0;
    float3 spot_specular = 0;
    for (int j = 0; j < 1; ++j)
    {
        float3 LP = pin.world_position.xyz - spot_light[j].position.xyz;
        float len = length(LP);
        if (len >= spot_light[j].range)
            continue;
        float attenuateLength = saturate(1.0f - len / spot_light[j].range);
        float attenuation = attenuateLength * attenuateLength;
        LP /= len;
        float3 spotDirection = normalize(spot_light[j].direction.xyz);
        float angle = dot(spotDirection, LP);
        float area = spot_light[j].innerCorn - spot_light[j].outerCorn;
        attenuation *= saturate(1.0f - (spot_light[j].innerCorn - angle) / area);
        //return float4(angle, 0, 0, 1);
        spot_diffuse += CalcLambert(N, LP, spot_light[j].color.rgb, float3(0.8f, 0.8f, 0.8f)) * attenuation;
        spot_specular += CalcPhongSpecular(N, LP, E, spot_light[j].color.rgb, float3(1.0f, 1.0f, 1.0f)) * attenuation;
    }
    return float4(spot_diffuse, 1);
    float4 color = float4(ambient, diffuse_color.a);
    color.rgb += diffuse_color.rgb * (directional_diffuse + point_diffuse + spot_diffuse);
    return (point_diffuse, 1);
    color.rgb += directional_specular + spot_specular + point_specular;

    return color;

}