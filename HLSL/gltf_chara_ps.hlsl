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
    static const float GammaFactor = 2.2f;

//-----------------------------------------
// サンプリング
//-----------------------------------------
    // 色
    float4 color = material_textures[BASECOLOR_TEXTURE].Sample(sampler_states[ANISOTROPIC], pin.texcoord);
    float alpha = color.a;
    color.rgb = pow(color.rgb, GammaFactor);
    // 金属度
    float metallic = material_textures[METALLIC_ROUGHNESS_TEXTURE].Sample(sampler_states[LINEAR], pin.texcoord).b;
    
    //// 滑らかさ
    //float smoothness = roughness_.Sample(sampler_states[ANISOTROPIC], pin.texcoord);
   
    // 法線マップ
    float3 normal = material_textures[NORMAL_TEXTURE].Sample(sampler_states[LINEAR], pin.texcoord);
    
    // 粗さ
    float roughness = material_textures[METALLIC_ROUGHNESS_TEXTURE].Sample(sampler_states[LINEAR], pin.texcoord).g;
    
    // マスク
    float mask = noise_map.Sample(sampler_states[ANISOTROPIC], pin.texcoord).x;
    
    // エミッシブ
    float4 emmisive = material_textures[EMISSIVE_TEXTURE].Sample(sampler_states[ANISOTROPIC], pin.texcoord);
    
    // オクルージョン
    float4 occlusion = material_textures[OCCLUSION_TEXTURE].Sample(sampler_states[LINEAR], pin.texcoord);
    float occlusion_factor = 1.0f;
    occlusion_factor *= occlusion;
    
//-----------------------------------------
//　ライティング
//-----------------------------------------   
    
    float3 finalLig = 0;
    {
        // 入射光のうち拡散反射になる割合
        float3 diffuseReflectance = lerp(color.rgb, 0.02f, metallic);
       
        // 垂直反射時のフレネル反射率
        float3 F0 = lerp(0.04, color.rgb, metallic);
        // 法線
        float3 N_ = normalize(pin.w_normal.xyz);
        // 接線ベクトル
        float3 T = has_tangent ? normalize(pin.w_tangent.xyz) : float3(1, 0, 0);
        float sigma = has_tangent ? pin.w_tangent.w : 1.0;
        T = normalize(T - N_ * dot(N_, T));
        // 従ベクトル
        float3 B = normalize(cross(N_, T) * sigma);
        // 法線マップからxyz成分を取得して( -1 ～ +1 )の間にスケーリング
        float3 normal_ = normal * 2.0f - 1.0f;
        // ワールド空間での法線ベクトルを得る
        float3 N = normalize(normal_.x * normalize(T) + normal_.y * normalize(B) + normal_.z * normalize(N_));
        // カメラから頂点への方向ベクトル
        float3 E = normalize(camera_position.xyz - pin.w_position.xyz);
  
	    // 視線ベクトル
        float3 V = normalize(pin.w_position.xyz - camera_position.xyz);
	
        
        float NdotV = max(0.0001f, dot(N, E));
        //return float4(NdotV, 0, 0, 1);
        
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
        PointLight(pin, diffuseReflectance, F0, N, V, roughness, pointDiffuse, pointSpecular);
        // 最終光に足し合わせる 
        finalLig += (pointDiffuse + pointSpecular);
        
        //-----------------------------------------
        // スポットライトのPBR
        //-----------------------------------------  
        float3 spotDiffuse = 0, spotSpecular = 0;
        SpotLight(pin, diffuseReflectance, F0, N, V, roughness, spotDiffuse, spotSpecular);
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
        finalLig += emmisive;
        
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
    
    
    finalLig.rgb = pow(finalLig.rgb, 1.0f / GammaFactor);
    return float4(finalLig, color.a);
};