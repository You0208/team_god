#include "fog_pbr.hlsli"

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
#define LINEAR_BORDER_BLACK 3
#define LINEAR_BORDER_WHITE 4

SamplerState sampler_states[3] : register(s0);
Texture2D texture_maps[6] : register(t0);

Texture2D noise_map : register(t9); // 先生ノイズは3D

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
    // ディフューズマップをサンプリング
    float4 color = texture_maps[0].Sample(sampler_states[ANISOTROPIC], pin.texcoord);
    float alpha = color.a;
    
    // 金属度
    float metallic = 0;
    // 滑らかさ
    float roughness = texture_maps[5].Sample(sampler_states[ANISOTROPIC], pin.texcoord).a;
    //// 調整
    //metallic = saturate(metallic + adjustMetalness);
    //smoothness = saturate(smoothness + adjustSmoothness);
    // 粗さ
    //float roughness = 1.0f - smoothness;
     //FOG
    {
        color = CalcFog(color, dis_fog_color, dis_fog_range.xy, length(pin.world_position.xyz - camera_position.xyz));
    }
    // 入射光のうち拡散反射になる割合
    float3 diffuseReflectance = lerp(color.rgb, 0.02f, metallic);
   // 垂直反射時のフレネル反射率
    float3 F0 = lerp(Dielectric, color.rgb, metallic);
    
    // 法線
    float3 N_ = normalize(pin.world_normal.xyz);
    // 接ベクトル
    float3 T = normalize(pin.world_tangent.xyz);
    float sigma = pin.world_tangent.w;
    // 接ベクトルは法線マップから計算され、法線ベクトル N と直交するように調整
    T = normalize(T - N_ * dot(N_, T));
    // 従ベクトル
    float3 B = normalize(cross(N_, T) * sigma);
    
    // 法線マップからxyz成分を取得して( -1 ～ +1 )の間にスケーリング
    float3 normal = texture_maps[1].Sample(sampler_states[LINEAR], pin.texcoord).xyz * 2.0f - 1.0f;
    float3 N = normalize(normal.x * normalize(T) + normal.y * normalize(B) + normal.z * normalize(N_));
    float3 E = normalize(camera_position.xyz - pin.world_position.xyz);
    // ライティング計算
    float3 directDiffuse = 0, directSpecular = 0;
    // 平行光源の処理
    DirectBDRF(diffuseReflectance, F0, N, E, light_direction.xyz,
         dirColor.rgb, roughness,
         directDiffuse, directSpecular);
    // 点光源の処理
    {
        float3 lightVector = pin.world_position.xyz - ptPosition.xyz;
        float lightLength = length(lightVector);
        // 距離減衰を算出する
        float attenuate = saturate(1.0f - lightLength / ptRange.x);
        lightVector = lightVector / lightLength;
        // 距離による影響率を計算する
        // ポイントライトとの距離を計算する
        float3 distance = length(pin.world_position.xyz - ptPosition.xyz);
        // 影響率は距離に比例して小さくなっていく
        float affect = 1.0f - 1.0f / ptRange.x * distance;
        // 影響力がマイナスにならないように補正をかける
        if (affect < 0.0f)
        {
            affect = 0.0f;
        }
        //影響を指数関数的にする。今回のサンプルでは3乗している
        affect = pow(affect, 3.0f);
        float3 diffuse = 0, specular = 0;
        DirectBDRF(diffuseReflectance, F0, N, E, lightVector,
        ptColor.rgb, roughness,
        diffuse, specular);
        directDiffuse += diffuse * attenuate;
        directSpecular += specular * attenuate;
    }
    
    // ディゾルブ
    {
        float mask = noise_map.Sample(sampler_states[ANISOTROPIC], pin.texcoord).x;
    
        // step関数を用いてmaskの値とdissolveThresholdの値を比較して透過値を0 or 1にする
        float dissolve = step(mask, threshold);
        // colorの透過値に乗算する
        //color.a *= mask;
        color.a *= dissolve;
        // アルファが0以下ならそもそも描画しないようにする
        clip(color.a - 0.01f);
    }
    
    	// SHADOW
#if 0
    const float shadow_depth_bias = 0.001;
#else
    float3 L = normalize(-light_direction.xyz);
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
    
	// EMISSIVE
    float3 emissive = texture_maps[2].Sample(sampler_states[LINEAR], pin.texcoord).rgb;


    const float emissive_intensity = 3.0;

    return float4((directDiffuse + directSpecular + emissive * emissive_intensity) * shadow_factor /*SHADOW*/, color.a);

}