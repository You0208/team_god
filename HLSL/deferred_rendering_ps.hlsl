#include "defefferd_light.hlsli"

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
#define LINEAR_BORDER_BLACK 3
#define LINEAR_BORDER_WHITE 4
SamplerState sampler_states[5] : register(s0);
Texture2D texture_maps[6] : register(t0);
float4 main(VS_OUT pin) : SV_TARGET
{
	// ガンマ係数
    static const float GammaFactor = 2.2f;
    
    // color
    float4 color = texture_maps[0].Sample(sampler_states[LINEAR_BORDER_BLACK], pin.texcoord);
    color.rgb = pow(color.rgb, GammaFactor);
    
    // Normal
    float3 normal = texture_maps[1].Sample(sampler_states[LINEAR_BORDER_BLACK], pin.texcoord).rgb;
    // position
    float3 position = texture_maps[2].Sample(sampler_states[LINEAR_BORDER_BLACK], pin.texcoord).rgb;
    float alpha = color.a;
    // 金属度
    float metallic = texture_maps[3].Sample(sampler_states[LINEAR_BORDER_BLACK], pin.texcoord).r;
    // 滑らかさ
    float smoothness = texture_maps[3].Sample(sampler_states[LINEAR_BORDER_BLACK], pin.texcoord).a;
    // エミッシブ
    float4 emmisive = texture_maps[4].Sample(sampler_states[LINEAR_BORDER_BLACK], pin.texcoord);    
    // オクルージョン
    float4 occlusion = texture_maps[5].Sample(sampler_states[LINEAR_BORDER_BLACK], pin.texcoord);
    
    // 粗さ
    float roughness = 1.0f - smoothness;
    
    //-----------------------------------------
    //　ライティング
    //-----------------------------------------   
    float3 finalLig = 0;
    {
        // 入射光のうち拡散反射になる割合
        float3 diffuseReflectance = lerp(color.rgb, 0.02f, metallic);
        // 垂直反射時のフレネル反射率
        float3 F0 = lerp(Dielectric, color.rgb, metallic);
        // ワールド空間での法線ベクトルを得る
        float3 N = normal.xyz;
	    // 視線ベクトル
        float3 V = normalize(position.xyz - camera_position.xyz);
        //-----------------------------------------
        // 直接光のPBR
        //-----------------------------------------   
        float3 directDiffuse = 0, directSpecular = 0;
        float3 L = normalize(directional_light_direction.xyz);
        
        DirectBDRF(diffuseReflectance, F0, N, V, L,
			directional_light_color.rgb, roughness, directDiffuse, directSpecular);
        
        // 最終光に足し合わせる
        finalLig += (directDiffuse + directSpecular);
         //-----------------------------------------
        // ポイントライトのPBR
        //-----------------------------------------  
        float3 pointDiffuse = 0, pointSpecular = 0;
        PointLight(position, diffuseReflectance, F0, N, V, roughness, pointDiffuse, pointSpecular);
        // 最終光に足し合わせる 
        finalLig += (pointDiffuse + pointSpecular);
        
        //-----------------------------------------
        // スポットライトのPBR
        //-----------------------------------------  
        float3 spotDiffuse = 0, spotSpecular = 0;
        SpotLight(position, diffuseReflectance, F0, N, V, roughness, spotDiffuse, spotSpecular);
        // 最終光に足し合わせる         
        finalLig += (spotDiffuse + spotSpecular);
    }
     // エミッシブ
    finalLig += emmisive;
        
    // ここの条件超適当
    // オクルージョン
    if (occlusion.x > 0)
        finalLig = lerp(finalLig, finalLig * occlusion.xyz, 1.0f);
    
    finalLig.rgb = pow(finalLig.rgb, 1.0f / GammaFactor);
    
#if 1
	// Tone mapping : HDR -> SDR
    const float exposure = 1.2;
    color.rgb = 1 - exp(-color.rgb * exposure);
#endif

    return float4(finalLig, alpha);
}
