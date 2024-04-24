


#include "defefferd_light.hlsli"
#include "gbuffer.hlsli"

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
#define LINEAR_BORDER_BLACK 3
#define LINEAR_BORDER_WHITE 4

Texture2D<float4> gbuffer_base_color : register(t0);
Texture2D<float4> gbuffer_emissive_color : register(t1);
Texture2D<float4> gbuffer_normal : register(t2);
Texture2D<float4> gbuffer_parameter : register(t3);
Texture2D<float> gbuffer_depth : register(t4);

SamplerState sampler_states[5] : register(s0);
//Texture2D texture_maps[4] : register(t0);

float4 main(VS_OUT pin) : SV_TARGET
{
    // GBufferテクスチャから情報をデコードする
    PSGBufferTextures gbuffer_textures;
    
    gbuffer_textures.base_color = gbuffer_base_color;
    gbuffer_textures.emissive_color = gbuffer_emissive_color;
    gbuffer_textures.normal = gbuffer_normal;
    gbuffer_textures.parameter = gbuffer_parameter;
    gbuffer_textures.depth = gbuffer_depth;
    gbuffer_textures.state = sampler_states[POINT];
    
    GBufferData data;
    data = DecodeGBuffer(gbuffer_textures, pin.texcoord, inv_view_projection);
  
	// ガンマ係数
    static const float GammaFactor = 2.2f;
    
    // color
    float4 color = gbuffer_textures.base_color.Sample(sampler_states[LINEAR_BORDER_BLACK], pin.texcoord);
    color.rgb = pow(color.rgb, GammaFactor);
    
    // Normal
    float3 normal = gbuffer_textures.normal.Sample(sampler_states[LINEAR_BORDER_BLACK], pin.texcoord).rgb;
#if 0
	// Gamma process
    const float GAMMA = 2.2f;
    color.rgb = pow(color.rgb, 1.0 / GAMMA);
#endif
    //-----------------------------------------
    //　ライティング
    //-----------------------------------------   
    float3 finalLig = 0;
    {
        // 入射光のうち拡散反射になる割合
        float3 diffuseReflectance = lerp(color.rgb, 0.02f, data.metalness);
        // 垂直反射時のフレネル反射率
        float3 F0 = lerp(Dielectric, color.rgb, data.metalness);
        // ワールド空間での法線ベクトルを得る
        float3 N = normalize(normal);
	    // 視線ベクトル
        float3 V = normalize(data.w_position.xyz - camera_position.xyz);
        return float4(V, 1);
        //-----------------------------------------
        // 直接光のPBR
        //-----------------------------------------   
        float3 directDiffuse = 0, directSpecular = 0;
        float3 L = normalize(directional_light_direction.xyz);
        
        DirectBDRF(diffuseReflectance, F0, N, V, L,
			directional_light_color.rgb, data.roughness, directDiffuse, directSpecular);
        // 最終光に足し合わせる
        finalLig += (directDiffuse + directSpecular);
  #if 1
         //-----------------------------------------
        // ポイントライトのPBR
        //-----------------------------------------  
        float3 pointDiffuse = 0, pointSpecular = 0;
        PointLight(data.w_position.xyz, diffuseReflectance, F0, N, V, data.roughness, pointDiffuse, pointSpecular);
        
        // 最終光に足し合わせる 
        finalLig += (pointDiffuse + pointSpecular);
        //-----------------------------------------
        // スポットライトのPBR
        //-----------------------------------------  
        float3 spotDiffuse = 0, spotSpecular = 0;
        SpotLight(data.w_position.xyz, diffuseReflectance, F0, N, V, data.roughness, spotDiffuse, spotSpecular);
        // 最終光に足し合わせる         
        finalLig += (spotDiffuse + spotSpecular);
#else        
        // 点光源
        for (int i = 0; i < 8; ++i)
        {
            float3 L = data.w_position.xyz - point_light[i].position.xyz;
            float len = length(L);
            
            if (len >= point_light[i].range)
                continue;
            
            float attenuateLength = saturate(1.0f - len / point_light[i].range);
            float attenuation = attenuateLength * attenuateLength;
            L /= len;
            
            float3 diffuse = (float3) 0, specular = (float3) 0;
            DirectBDRF(diffuseReflectance, F0, N, V, L, point_light[i].color.rgb, data.roughness, diffuse, specular);
            
            finalLig += diffuse * attenuation;
            finalLig += specular * attenuation;
        }
        
        
        
    #endif
    }
#if 1
	// Tone mapping : HDR -> SDR
    const float exposure = 1.2;
    color.rgb = 1 - exp(-color.rgb * exposure);
#endif

    
    finalLig.rgb = pow(finalLig.rgb, 1.0f / GammaFactor);
    return float4(finalLig, 1);
}
