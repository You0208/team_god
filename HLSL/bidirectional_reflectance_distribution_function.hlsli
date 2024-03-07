#ifndef __BIDIRECTIONAL_REFLECTANCE_DISTRIBUTION_FUNCTION_HLSL__
#define __BIDIRECTIONAL_REFLECTANCE_DISTRIBUTION_FUNCTION_HLSL__

#include "image_based_lighting.hlsli"

//--------------------------------------------
// フレネル項
//--------------------------------------------
// F0 : 垂直入射時の反射率
// F90 : ?
// VoH: 視線ベクトルとハーフベクトル（光源へのベクトルと視点へのベクトルの中間ベクトル）
float3 f_schlick(float3 f0, float3 f90, float VoH)
{
    return f0 + (f90 - f0) * pow(clamp(1.0 - VoH, 0.0, 1.0), 5.0);
}

//--------------------------------------------
// 幾何減衰項の算出(GGX)
//--------------------------------------------
// NoL：法線ベクトルと光源へのベクトルとの内積
// NoV：法線ベクトルと視線へのベクトルとの内積
// alpha_roughness：粗さ
float v_ggx(float NoL, float NoV, float alpha_roughness)
{
    float alpha_roughness_sq = alpha_roughness * alpha_roughness;
    
    float ggxv = NoL * sqrt(NoV * NoV * (1.0 - alpha_roughness_sq) + alpha_roughness_sq);
    float ggxl = NoV * sqrt(NoL * NoL * (1.0 - alpha_roughness_sq) + alpha_roughness_sq);
    
    float ggx = ggxv + ggxl;
    return (ggx > 0.0) ? 0.5 / ggx : 0.0;
}

//--------------------------------------------
// 法線分布関数(GGX)
//--------------------------------------------
// NoH : 法線ベクトルとハーフベクトル（光源へのベクトルと視点へのベクトルの中間ベクトル）の内積
// alpha_roughness : 粗さ
float d_ggx(float NoH, float alpha_roughness)
{
    const float PI = 3.14159265358979;
    float alpha_roughness_sq = alpha_roughness * alpha_roughness;
    float f = (NoH * NoH) * (alpha_roughness_sq - 1.0) + 1.0;
    return alpha_roughness_sq / (PI * f * f);
}

//--------------------------------------------
// 拡散反射BRDF（Lambertian diffuse BRDF）
//--------------------------------------------
// f0 : 垂直入射時の反射率
// f90 : ?
// diffuse_color : ライトカラー
// VoH: 視線ベクトルとハーフベクトル（光源へのベクトルと視点へのベクトルの中間ベクトル）
float3 brdf_lambertian(float3 f0, float3 f90, float3 diffuse_color, float VoH)
{
    const float PI = 3.14159265358979;
    return (1.0 - f_schlick(f0, f90, VoH)) * (diffuse_color / PI);
}

//--------------------------------------------
// 鏡面BRDF（Walter）
//--------------------------------------------
// f0 : 垂直入射時の反射率
// f90 : ?
// alpha_roughness : 粗さ
// VoH: 視線ベクトルとハーフベクトル（光源へのベクトルと視点へのベクトルの中間ベクトル）
// NoL: 法線ベクトルと光源へのベクトルとの内積
// NoV: 法線ベクトルと視線へのベクトルとの内積
// NoH: 法線ベクトルとハーフベクトル（光源へのベクトルと視点へのベクトルの中間ベクトル）の内積
float3 brdf_specular_ggx(float3 f0, float3 f90, float alpha_roughness, float VoH, float NoL, float NoV, float NoH)
{
    float3 F = f_schlick(f0, f90, VoH);
    float Vis = v_ggx(NoL, NoV, alpha_roughness);
    float D = d_ggx(NoH, alpha_roughness);

    return F * Vis * D;
}

float3 ibl_radiance_lambertian(float3 N, float3 V, float roughness, float3 diffuse_color, float3 f0)
{
    float NoV = clamp(dot(N, V), 0.0, 1.0);
    
    float2 brdf_sample_point = clamp(float2(NoV, roughness), 0.0, 1.0);
    float2 f_ab = sample_lut_ggx(brdf_sample_point).rg;

    float3 irradiance = sample_diffuse_iem(N).rgb;

    float3 fr = max(1.0 - roughness, f0) - f0;
    float3 k_s = f0 + fr * pow(1.0 - NoV, 5.0);
    float3 fss_ess = k_s * f_ab.x + f_ab.y;
    
    float ems = (1.0 - (f_ab.x + f_ab.y));
    float3 f_avg = (f0 + (1.0 - f0) / 21.0);
    float3 fms_ems = ems * fss_ess * f_avg / (1.0 - f_avg * ems);
    float3 k_d = diffuse_color * (1.0 - fss_ess + fms_ems);
    
    return (fms_ems + k_d) * irradiance;
}

float3 ibl_radiance_ggx(float3 N, float3 V, float roughness, float3 f0)
{
    float NoV = clamp(dot(N, V), 0.0, 1.0);
    
    float2 brdf_sample_point = clamp(float2(NoV, roughness), 0.0, 1.0);
    float2 f_ab = sample_lut_ggx(brdf_sample_point).rg;

    float3 R = normalize(reflect(-V, N));
    float3 specular_light = sample_specular_pmrem(R, roughness).rgb;
    
    float3 fr = max(1.0 - roughness, f0) - f0;
    float3 k_s = f0 + fr * pow(1.0 - NoV, 5.0);
    float3 fss_ess = k_s * f_ab.x + f_ab.y;
    
    return specular_light * fss_ess;
}

#endif