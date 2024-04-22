#include "light.hlsli"

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
#define LINEAR_BORDER_BLACK 3
#define LINEAR_BORDER_WHITE 4

SamplerState sampler_states[3] : register(s0);
Texture2D texture_maps[8] : register(t0);

Texture2D metalSmoothTex : register(t4);

//Texture2D d_color : register(t20);
//Texture2D d_normal : register(t21);
//Texture2D d_MS : register(t22);

Texture2D noise_map : register(t9); // 先生ノイズは3D

// SHADOW
SamplerComparisonState comparison_sampler_state : register(s5);
Texture2D shadow_map : register(t8);


float4 main(VS_OUT pin) : SV_TARGET
{
//-----------------------------------------
// サンプリング
//-----------------------------------------
    static const float GAMMA = 2.2;

    // 色
    float4 color = texture_maps[0].Sample(sampler_states[ANISOTROPIC], pin.texcoord);
    color.rgb = pow(color.rgb, GAMMA);
    float alpha = color.a;
    
    // 金属度
    float metallic = metalSmoothTex.Sample(sampler_states[ANISOTROPIC], pin.texcoord).r;
    
    
    // 滑らかさ
    float smoothness = metalSmoothTex.Sample(sampler_states[ANISOTROPIC], pin.texcoord).a;
    
    
    // 法線マップ
    float3 normal = texture_maps[1].Sample(sampler_states[LINEAR], pin.texcoord);
    
    // 粗さ
    float roughness = 1.0f - smoothness;
    
    // マスク
    float mask = noise_map.Sample(sampler_states[ANISOTROPIC], pin.texcoord).x;
    
//-----------------------------------------
//　ライティング
//-----------------------------------------   
    
    float3 finalLig = 0;
    {
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
        float3 normal_ = normal * 2.0f - 1.0f;
        // ワールド空間での法線ベクトルを得る
        float3 N = normalize(normal_.x * normalize(T) + normal_.y * normalize(B) + normal_.z * normalize(N_));
        // カメラから頂点への方向ベクトル
        float3 E = normalize(camera_position.xyz - pin.world_position.xyz);
        // 視線ベクトル
        float3 V = normalize(pin.world_position.xyz - camera_position.xyz);
	
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
        SpotLight(pin, diffuseReflectance, F0, N, E, roughness, spotDiffuse, spotSpecular);
        // 最終光に足し合わせる         
        finalLig += (spotDiffuse + spotSpecular);
        
        //-----------------------------------------
        //　シャドウ
        //----------------------------------------- 
    
        float3 shadow_factor = 1.0f;
        Shadow(pin, normal, T, B, shadow_map, comparison_sampler_state, shadow_factor);
        // 最終光に足し合わせる    
        finalLig *= shadow_factor;
        
        //-----------------------------------------
        //　リムライト
        //----------------------------------------- 
        float3 rimColor = 0, hemiLight = 0;
        LimHemiLight(pin, normal, N_, T, E, rimColor, hemiLight);
        // 最終光に足し合わせる     
        finalLig += (rimColor + hemiLight);
    }
 
//-----------------------------------------
//　ディゾルブ
//----------------------------------------- 
    {
        float dissolve = step(mask, threshold);
        color.a *= dissolve;
        // アルファが0以下ならそもそも描画しないようにする
        clip(color.a - 0.01f);
    }
    
    
    finalLig = pow(finalLig, 1.0f / GAMMA);
    return float4(finalLig, color.a);
};