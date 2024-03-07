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
/// Lambert�g�U���ˌ����v�Z����
/// </summary>
float3 CalcLambertDiffuse(float3 lightDirection, float3 lightColor, float3 normal)
{
    // �s�N�Z���̖@���ƃ��C�g�̕����̓��ς��v�Z����
    float t = dot(normal, lightDirection) * -1.0f;

    // ���ς̒l��0�ȏ�̒l�ɂ���
    t = max(0.0f, t);

    // �g�U���ˌ����v�Z����
    return lightColor * t;
}

//--------------------------------------------
// �������C�g
//--------------------------------------------
// N:�@��(���K���ς�)
// E:���_�����x�N�g��(���K���ς�)
// L:���˃x�N�g��(���K���ς�)
// C :���C�g�F
// RimPower : �������C�g�̋���(�����l�̓e�L�g�[�Ȃ̂Ŏ����Őݒ肷�邪�g)
float3 CalcRimLight(float3 normal, float3 E, float3 L, float3 lightColor, float RimPower = 3.0f)
{
    float rim = 1.0f - saturate(dot(normal, -E));
    return lightColor * pow(rim, RimPower) * saturate(dot(L, -E));
}

/// <summary>
/// Phong���ʔ��ˌ����v�Z����
/// </summary>
float3 CalcPhongSpecular(float3 lightDirection, float3 lightColor, float3 worldPos, float3 normal)
{
    // ���˃x�N�g�������߂�
    float3 refVec = reflect(lightDirection, normal);

    // �������������T�[�t�F�C�X���王�_�ɐL�т�x�N�g�������߂�
    float3 toEye = camera_position.xyz - worldPos;
    toEye = normalize(toEye);

    // ���ʔ��˂̋��������߂�
    float t = dot(refVec, toEye);

    // ���ʔ��˂̋�����0�ȏ�̐��l�ɂ���
    t = max(0.0f, t);

    // ���ʔ��˂̋������i��
    t = pow(t, 5.0f);

    // ���ʔ��ˌ������߂�
    return lightColor * t;
}


// �U�d��(������ł��Œ�4%(0.0f4)�͋��ʔ��˂���)
static const float Dielectric = 0.04f;
//--------------------------------------------
// �g�U����BRDF(���K�������o�[�g�̊g�U����)
//--------------------------------------------
// diffuseReflectance : ���ˌ��̂����g�U���˂ɂȂ銄��
float3 DiffuseBRDF(float3 diffuseReflectance)
{
    return diffuseReflectance / PI;
}

//--------------------------------------------
// �t���l����
//--------------------------------------------
// F0 : �������ˎ��̔��˗�
// VdotH: �����x�N�g���ƃn�[�t�x�N�g���i�����ւ̃x�N�g���Ǝ��_�ւ̃x�N�g���̒��ԃx�N�g��
float3 CalcFresnel(float3 F0, float VdotH)
{
    return F0 + (1.0f - F0) * pow(1.0f - VdotH, 5.0f);
}
//--------------------------------------------
// �@�����z�֐�
//--------------------------------------------
// NdotH : �@���x�N�g���ƃn�[�t�x�N�g���i�����ւ̃x�N�g���Ǝ��_�ւ̃x�N�g���̒��ԃx�N�g���j�̓���
// roughness : �e��
float CalcNormalDistributionFunction(float NdotH, float roughness)
{
    float alpha = roughness * roughness;
    float alpha2 = alpha * alpha;
    float denom = (NdotH * NdotH) * (alpha2 - 1.0f) + 1.0f;
    return alpha2 / (PI * denom * denom);
}
//--------------------------------------------
// �􉽌������̎Z�o
//--------------------------------------------
// Ndotv : �@���Ƒ��x�N�g���Ƃ̓���
// k : �e��/2
float CalcGeometryFunction(float Ndotv, float k)
{
    return Ndotv / (Ndotv * (1.0 - k) + k);
}
// NdotL : �@���x�N�g���ƌ����ւ̃x�N�g���Ƃ̓���
// NdotV : �@���x�N�g���Ǝ����ւ̃x�N�g���Ƃ̓���
// roughness : �e��
float CalcGeometryFunction(float NdotL, float NdotV, float roughness)
{
    float r = roughness * 0.5f;
    float shadowing = CalcGeometryFunction(NdotL, r);
    float masking = CalcGeometryFunction(NdotV, r);
    return shadowing * masking;
}
//--------------------------------------------
// ���ʔ���BRDF�i�N�b�N�E�g�����X�̃}�C�N���t�@�Z�b�g���f���j
//--------------------------------------------
// NdotV : �@���x�N�g���Ǝ����ւ̃x�N�g���Ƃ̓���
// NdotL : �@���x�N�g���ƌ����ւ̃x�N�g���Ƃ̓���
// NdotH : �@���x�N�g���ƃn�[�t�x�N�g���Ƃ̓���
// VdotH : �����ւ̃x�N�g���ƃn�[�t�x�N�g���Ƃ̓���
// fresnelF0 : �������ˎ��̃t���l�����ːF
// roughness : �e��
float3 SpecularBRDF(float NdotV, float NdotL, float NdotH, float VdotH, float3 fresnelF0, float roughness)
{
 // D��(�@�����z)
    float D = CalcNormalDistributionFunction(NdotH, roughness);
 // G��(�􉽌�����)
    float G = CalcGeometryFunction(NdotL, NdotV, roughness);
 // F��(�t���l������)
    float3 F = CalcFresnel(fresnelF0, VdotH);
    return (D * F * G) / max(0.0001f, 4 * NdotL * NdotV);
}
//--------------------------------------------
// ���ڌ��̕����x�[�X���C�e�B���O
//--------------------------------------------
// diffuseReflectance : ���ˌ��̂����g�U���˂ɂȂ銄��
// F0 : �������ˎ��̃t���l�����ːF
// normal : �@���x�N�g��(���K���ς�)
// eyeVector : ���_�Ɍ������x�N�g��(���K���ς�)
// lightVector : �����Ɍ������x�N�g��(���K���ς�)
// lightColor : ���C�g�J���[
// roughness : �e��
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
// ���K�V�[�ȃ��C�e�B���O�Ƃ̌݊�����ۂꍇ��PI�ŏ�Z����
    irradiance *= PI;
// �g�U����BRDF(���K�������o�[�g�̊g�U����)
    outDiffuse = DiffuseBRDF(diffuseReflectance) * irradiance;
// ���ʔ���BRDF�i�N�b�N�E�g�����X�̃}�C�N���t�@�Z�b�g���f���j
    outSpecular = SpecularBRDF(NdotV, NdotL, NdotH, VdotH, F0, roughness) * irradiance;
}
//--------------------------------------------
// �t�H�O
//--------------------------------------------
//color:���݂̃s�N�Z���F
//fog_color:�t�H�O�̐F
//fog_range:�t�H�O�͈̔͏��
//eye_length:���_����̋���
float4 CalcFog(in float4 color, float4 fog_color, float2 fog_range, float eye_length)
{
    float fogAlpha = saturate((eye_length - fog_range.x) / (fog_range.y - fog_range.x));
    return lerp(color, fog_color, fogAlpha);
}
