#include "gltf_model_my.hlsli"
#include "bidirectional_reflectance_distribution_function.hlsli"

#define BASECOLOR_TEXTURE 0
#define METALLIC_ROUGHNESS_TEXTURE 1
#define NORMAL_TEXTURE 2
#define EMISSIVE_TEXTURE 3
#define OCCLUSION_TEXTURE 4

Texture2D<float4> material_textures[5] : register(t1);


Texture2D noise_map : register(t9); // 先生ノイズは3D

// SHADOW
SamplerComparisonState comparison_sampler_state : register(s5);
Texture2D shadow_map : register(t8);


float4 main(VS_OUT pin) : SV_TARGET
{
	// ガンマ係数
    static const float GAMMA = 2.2f;

//-----------------------------------------
// サンプリング
//-----------------------------------------
    // 色
    material_constants m = materials[material];
    
    float4 basecolor_factor = m.pbr_metallic_roughness.basecolor_factor;
    const int basecolor_texture = m.pbr_metallic_roughness.basecolor_texture.index;
    if (basecolor_texture > -1)
    {
        float4 sampled = material_textures[BASECOLOR_TEXTURE].Sample(sampler_states[ANISOTROPIC], pin.texcoord);
        sampled.rgb = pow(sampled.rgb, GAMMA);
        basecolor_factor *= sampled;
       // return sampled;
    }
    //return material_textures[BASECOLOR_TEXTURE].Sample(sampler_states[ANISOTROPIC], pin.texcoord);
    
    // 法線マップ
    float3 normal = material_textures[NORMAL_TEXTURE].Sample(sampler_states[LINEAR], pin.texcoord);
    
    float roughness_factor = m.pbr_metallic_roughness.roughness_factor;
    float metallic_factor = m.pbr_metallic_roughness.metallic_factor;
    const int metallic_roughness_texture = m.pbr_metallic_roughness.metallic_roughness_texture.index;
    if (metallic_roughness_texture > -1)
    {
        float4 sampled = material_textures[METALLIC_ROUGHNESS_TEXTURE].Sample(sampler_states[LINEAR], pin.texcoord);
        roughness_factor *= sampled.g;
        metallic_factor *= sampled.b;
    }
    // エミッシブ
    float3 emmisive_factor = m.emissive_factor;
    const int emissive_texture = m.emissive_texture.index;
    if (emissive_texture > -1)
    {
        float4 sampled = material_textures[EMISSIVE_TEXTURE].Sample(sampler_states[ANISOTROPIC], pin.texcoord);
        sampled.rgb = pow(sampled.rgb, GAMMA);
        emmisive_factor *= sampled.rgb;
    }
    
    float occlusion_factor = 1.0;
    const int occlusion_texture = m.occlusion_texture.index;
    if (occlusion_texture > -1)
    {
        float4 sampled = material_textures[OCCLUSION_TEXTURE].Sample(sampler_states[LINEAR], pin.texcoord);
        occlusion_factor *= sampled.r;
    }
    const float occlusion_strength = m.occlusion_texture.strength;
    
    
    // マスク
    float mask = noise_map.Sample(sampler_states[ANISOTROPIC], pin.texcoord).x;
    
//-----------------------------------------
//　ライティング
//-----------------------------------------   
    
    float3 finalLig = 0;
    {
        // 入射光のうち拡散反射になる割合
        float3 diffuseReflectance = lerp(basecolor_factor.rgb, 0.02f, metallic_factor);
       
        // 垂直反射時のフレネル反射率
        float3 F0 = lerp(0.04, basecolor_factor.rgb, metallic_factor);
        // 法線
        float3 N = normalize(pin.w_normal.xyz);
        // 接線ベクトル
        float3 T = has_tangent ? normalize(pin.w_tangent.xyz) : float3(1, 0, 0);
        float sigma = has_tangent ? pin.w_tangent.w : 1.0;
        T = normalize(T - N * dot(N, T));
        // 従ベクトル
        float3 B = normalize(cross(N, T) * sigma);

        const int normal_texture = m.normal_texture.index;
        if (normal_texture > -1)
        {
            float4 sampled = material_textures[NORMAL_TEXTURE].Sample(sampler_states[LINEAR], pin.texcoord);
            float3 normal_factor = sampled.xyz;
            normal_factor = (normal_factor * 2.0) - 1.0;
            normal_factor = normalize(normal_factor * float3(m.normal_texture.scale, m.normal_texture.scale, 1.0));
            N = normalize((normal_factor.x * T) + (normal_factor.y * B) + (normal_factor.z * N));
        }
        
	    // 視線ベクトル
        float3 V = normalize(pin.w_position.xyz - camera_position.xyz);
	
        //-----------------------------------------
        // 直接光のPBR
        //-----------------------------------------   
        float3 directDiffuse = 0, directSpecular = 0;
        float3 L = normalize(directional_light_direction.xyz);
        DirectBDRF(diffuseReflectance, F0, N, V, L,
			directional_light_color.rgb, roughness_factor, directDiffuse, directSpecular);
        // 最終光に足し合わせる
        finalLig += (directDiffuse + directSpecular);
        //-----------------------------------------
        // ポイントライトのPBR
        //-----------------------------------------  
        float3 pointDiffuse = 0, pointSpecular = 0;
        PointLight(pin, diffuseReflectance, F0, N, V, roughness_factor, pointDiffuse, pointSpecular);
        // 最終光に足し合わせる 
        finalLig += (pointDiffuse + pointSpecular);
        
        //-----------------------------------------
        // スポットライトのPBR
        //-----------------------------------------  
        float3 spotDiffuse = 0, spotSpecular = 0;
        SpotLight(pin, diffuseReflectance, F0, N, V, roughness_factor, spotDiffuse, spotSpecular);
        // 最終光に足し合わせる         
        finalLig += (spotDiffuse + spotSpecular);
        
        //-----------------------------------------
        //　シャドウ
        //----------------------------------------- 
    
        float3 shadow_factor = 1.0f;
        Shadow(pin, normal, T, B, shadow_map, comparison_sampler_state, shadow_factor);
        // 最終光に足し合わせる    
        finalLig *= shadow_factor;
        
        // エミッシブ
        finalLig += emmisive_factor;
        
        // オクルージョン
        finalLig = lerp(finalLig, finalLig * occlusion_factor, 1.0f);
        //-----------------------------------------
        //　リムライト
        //----------------------------------------- 
        //float3 rimColor = 0, hemiLight = 0;
        //LimHemiLight(pin, normal, N_, T, E, rimColor, hemiLight);
        //// 最終光に足し合わせる     
        //finalLig += (rimColor + hemiLight);
    }
 
//-----------------------------------------
//　ディゾルブ
//----------------------------------------- 
    //{
    //    float dissolve = step(mask, threshold);
    //    color.a *= dissolve;
    //    // アルファが0以下ならそもそも描画しないようにする
    //    clip(color.a - 0.01f);
    //}
    
    
    finalLig.rgb = pow(finalLig.rgb, 1.0f / GAMMA);
    return float4(finalLig, basecolor_factor.a);
};