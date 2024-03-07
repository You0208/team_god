#include "ConstantBuffer.hlsli"
struct VS_IN
{
    float4 position : POSITION;
    float4 normal : NORMAL;
    float4 tangent : TANGENT;
    float2 texcoord : TEXCOORD;
    float4 bone_weights : WEIGHTS;
    uint4 bone_indices : BONES;
};

struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 world_position : POSITION;
    float4 world_normal : NORMAL;
    float4 world_tangent : TANGENT;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
};

#define  NONE 0
#define SCENE 1 
#define FOG 2
#define MASK 3
#define PBR 4
#define D_FOG 5
#define LIGHT 6
#define SHADPW 7

/// <summary>
/// Lambert拡散反射光を計算する
/// </summary>
float3 CalcLambertDiffuse(float3 lightDirection, float3 lightColor, float3 normal)
{
    // ピクセルの法線とライトの方向の内積を計算する
    float t = dot(normal, lightDirection) * -1.0f;

    // 内積の値を0以上の値にする
    t = max(0.0f, t);

    // 拡散反射光を計算する
    return lightColor * t;
}

//--------------------------------------------
// リムライト
//--------------------------------------------
// N:法線(正規化済み)
// E:視点方向ベクトル(正規化済み)
// L:入射ベクトル(正規化済み)
// C :ライト色
// RimPower : リムライトの強さ(初期値はテキトーなので自分で設定するが吉)
float3 CalcRimLight(float3 normal, float3 E, float3 L, float3 lightColor, float RimPower = 3.0f)
{
    float rim = 1.0f - saturate(dot(normal, -E));
    return lightColor * pow(rim, RimPower) * saturate(dot(L, -E));
}

/// <summary>
/// Phong鏡面反射光を計算する
/// </summary>
float3 CalcPhongSpecular(float3 lightDirection, float3 lightColor, float3 worldPos, float3 normal)
{
    // 反射ベクトルを求める
    float3 refVec = reflect(lightDirection, normal);

    // 光が当たったサーフェイスから視点に伸びるベクトルを求める
    float3 toEye = camera_position.xyz - worldPos;
    toEye = normalize(toEye);

    // 鏡面反射の強さを求める
    float t = dot(refVec, toEye);

    // 鏡面反射の強さを0以上の数値にする
    t = max(0.0f, t);

    // 鏡面反射の強さを絞る
    t = pow(t, 5.0f);

    // 鏡面反射光を求める
    return lightColor * t;
}


// 誘電率(非金属でも最低4%(0.0f4)は鏡面反射する)
static const float Dielectric = 0.04f;
//--------------------------------------------
// 拡散反射BRDF(正規化ランバートの拡散反射)
//--------------------------------------------
// diffuseReflectance : 入射光のうち拡散反射になる割合
float3 DiffuseBRDF(float3 diffuseReflectance)
{
    return diffuseReflectance / PI;
}

//--------------------------------------------
// フレネル項
//--------------------------------------------
// F0 : 垂直入射時の反射率
// VdotH: 視線ベクトルとハーフベクトル（光源へのベクトルと視点へのベクトルの中間ベクトル
float3 CalcFresnel(float3 F0, float VdotH)
{
    return F0 + (1.0f - F0) * pow(1.0f - VdotH, 5.0f);
}
//--------------------------------------------
// 法線分布関数
//--------------------------------------------
// NdotH : 法線ベクトルとハーフベクトル（光源へのベクトルと視点へのベクトルの中間ベクトル）の内積
// roughness : 粗さ
float CalcNormalDistributionFunction(float NdotH, float roughness)
{
    float alpha = roughness * roughness;
    float alpha2 = alpha * alpha;
    float denom = (NdotH * NdotH) * (alpha2 - 1.0f) + 1.0f;
    return alpha2 / (PI * denom * denom);
}
//--------------------------------------------
// 幾何減衰項の算出
//--------------------------------------------
// Ndotv : 法線と他ベクトルとの内積
// k : 粗さ/2
float CalcGeometryFunction(float Ndotv, float k)
{
    return Ndotv / (Ndotv * (1.0 - k) + k);
}
// NdotL : 法線ベクトルと光源へのベクトルとの内積
// NdotV : 法線ベクトルと視線へのベクトルとの内積
// roughness : 粗さ
float CalcGeometryFunction(float NdotL, float NdotV, float roughness)
{
    float r = roughness * 0.5f;
    float shadowing = CalcGeometryFunction(NdotL, r);
    float masking = CalcGeometryFunction(NdotV, r);
    return shadowing * masking;
}
//--------------------------------------------
// 鏡面反射BRDF（クック・トランスのマイクロファセットモデル）
//--------------------------------------------
// NdotV : 法線ベクトルと視線へのベクトルとの内積
// NdotL : 法線ベクトルと光源へのベクトルとの内積
// NdotH : 法線ベクトルとハーフベクトルとの内積
// VdotH : 視線へのベクトルとハーフベクトルとの内積
// fresnelF0 : 垂直入射時のフレネル反射色
// roughness : 粗さ
float3 SpecularBRDF(float NdotV, float NdotL, float NdotH, float VdotH, float3 fresnelF0, float roughness)
{
 // D項(法線分布)
    float D = CalcNormalDistributionFunction(NdotH, roughness);
 // G項(幾何減衰項)
    float G = CalcGeometryFunction(NdotL, NdotV, roughness);
 // F項(フレネル反射)
    float3 F = CalcFresnel(fresnelF0, VdotH);
    return (D * F * G) / max(0.0001f, 4 * NdotL * NdotV);
}
//--------------------------------------------
// 直接光の物理ベースライティング
//--------------------------------------------
// diffuseReflectance : 入射光のうち拡散反射になる割合
// F0 : 垂直入射時のフレネル反射色
// normal : 法線ベクトル(正規化済み)
// eyeVector : 視点に向かうベクトル(正規化済み)
// lightVector : 光源に向かうベクトル(正規化済み)
// lightColor : ライトカラー
// roughness : 粗さ
void DirectBDRF(float3 diffuseReflectance,
float3 F0,
float3 normal,
float3 eyeVector,
float3 lightVector,
float3 lightColor,
float roughness,
out float3 outDiffuse,
out float3 outSpecular)
{
    float3 N = normal;
    float3 L = -lightVector;
    float3 V = eyeVector;
    float3 H = normalize(L + V);
    float NdotV = max(0.0001f, dot(N, V));
    float NdotL = max(0.0001f, dot(N, L));
    float NdotH = max(0.0001f, dot(N, H));
    float VdotH = max(0.0001f, dot(V, H));
    float3 irradiance = lightColor * NdotL;
// レガシーなライティングとの互換性を保つ場合はPIで乗算する
    irradiance *= PI;
// 拡散反射BRDF(正規化ランバートの拡散反射)
    outDiffuse = DiffuseBRDF(diffuseReflectance) * irradiance;
// 鏡面反射BRDF（クック・トランスのマイクロファセットモデル）
    outSpecular = SpecularBRDF(NdotV, NdotL, NdotH, VdotH, F0, roughness) * irradiance;
}
//--------------------------------------------
// フォグ
//--------------------------------------------
//color:現在のピクセル色
//fog_color:フォグの色
//fog_range:フォグの範囲情報
//eye_length:視点からの距離
float4 CalcFog(in float4 color, float4 fog_color, float2 fog_range, float eye_length)
{
    float fogAlpha = saturate((eye_length - fog_range.x) / (fog_range.y - fog_range.x));
    return lerp(color, fog_color, fogAlpha);
}
