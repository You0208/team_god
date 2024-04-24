#include "gltf_model.hlsli"

#define BASECOLOR_TEXTURE 0
#define METALLIC_ROUGHNESS_TEXTURE 1
#define NORMAL_TEXTURE 2
#define EMISSIVE_TEXTURE 3
#define OCCLUSION_TEXTURE 4

//ピクセルシェーダーの出力構造体
struct PSOutPut
{
    float4 Color : SV_Target0;
    float4 Normal : SV_Target1;
    float4 Position : SV_Target2;
    float4 metalSmooth : SV_Target3; // 金属度と滑らかさ。xに金属度、wに滑らかさが出力される。
    float4 emissive : SV_Target4;
    float4 occlusion : SV_Target5;
};

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
// SHADOW
#define LINEAR_BORDER_BLACK 3
#define LINEAR_BORDER_WHITE 4

Texture2D<float4> material_textures[5] : register(t1);

SamplerState sampler_states[5] : register(s0);

//モデル用のピクセルシェーダー
PSOutPut main(VS_OUT pin)
{
    PSOutPut output;
        // 色
    float4 base_color = material_textures[BASECOLOR_TEXTURE].Sample(sampler_states[ANISOTROPIC], pin.texcoord);
    output.Color = base_color;
    
    //// 従ベクトル
    //float3 N = normalize(pin.w_normal.xyz);
    //// 接ベクトル 
    //float3 T = normalize(pin.w_tangent.xyz);
    //float sigma = pin.w_tangent.w;
    
    //T = normalize(T - N * dot(N, T));
    //float3 B = normalize(cross(N, T) * sigma);

    float4 normal = material_textures[NORMAL_TEXTURE].Sample(sampler_states[LINEAR], pin.texcoord);
    //normal = (normal * 2.0) - 1.0;
    //N = normalize((normal.x * normalize(T)) + (normal.y * normalize(B)) + (normal.z * normalize(N)));
    
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
        
    output.Normal = float4(N, 0);
    
    output.Position = pin.w_position;
    
    output.metalSmooth.r = material_textures[METALLIC_ROUGHNESS_TEXTURE].Sample(sampler_states[ANISOTROPIC], pin.texcoord).b;
    output.metalSmooth.a = 1.0f - material_textures[METALLIC_ROUGHNESS_TEXTURE].Sample(sampler_states[ANISOTROPIC], pin.texcoord).g;
    
    
    // エミッシブ
    
    // エミッシブ
    output.emissive = material_textures[EMISSIVE_TEXTURE].Sample(sampler_states[ANISOTROPIC], pin.texcoord);
    
    // オクルージョン
    output.occlusion = material_textures[OCCLUSION_TEXTURE].Sample(sampler_states[LINEAR], pin.texcoord);
    return output;
}