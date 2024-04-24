#include "light.hlsli"
//#include "gbuffer.hlsli"

//#define POINT 0
//#define LINEAR 1
//#define ANISOTROPIC 2
//// SHADOW
//#define LINEAR_BORDER_BLACK 3
//#define LINEAR_BORDER_WHITE 4

//SamplerState sampler_states[5] : register(s0);
//Texture2D texture_maps[4] : register(t0);
//Texture2D metalSmoothTex : register(t4);

//PSGBufferOut main(VS_OUT pin, bool is_front_face : SV_IsFrontFace)
//{
 
//    // GBufferData�ɏo�͏����܂Ƃ߂�
//    GBufferData data;
//    float4 albedoColor = texture_maps[0].Sample(sampler_states[ANISOTROPIC], pin.texcoord);
//    data.base_color = albedoColor;
    
//    // �]�x�N�g��
//    float3 N = normalize(pin.world_normal.xyz);
//    // �ڃx�N�g�� 
//    float3 T = normalize(pin.world_tangent.xyz);
//    float sigma = pin.world_tangent.w;
    
//    T = normalize(T - N * dot(N, T));
//    float3 B = normalize(cross(N, T) * sigma);

//    float4 normal = texture_maps[1].Sample(sampler_states[LINEAR], pin.texcoord);
//    normal = (normal * 2.0) - 1.0;
//    N = normalize((normal.x * T) + (normal.y * B) + (normal.z * N));
    
//    data.w_normal = float4(N, 0);
    
//    data.w_position = pin.world_position;
    
//    data.roughness = 1.0f - metalSmoothTex.Sample(sampler_states[ANISOTROPIC], pin.texcoord).a;
//    data.metalness = metalSmoothTex.Sample(sampler_states[ANISOTROPIC], pin.texcoord).r;

//    return EncodeGBuffer(data, view_projection);
//}

//�s�N�Z���V�F�[�_�[�̏o�͍\����
struct PSOutPut
{
    float4 Color : SV_Target0;
    float4 Normal : SV_Target1;
    float4 Position : SV_Target2;
    float4 metalSmooth : SV_Target3; // �����x�Ɗ��炩���Bx�ɋ����x�Aw�Ɋ��炩�����o�͂����B
};

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
// SHADOW
#define LINEAR_BORDER_BLACK 3
#define LINEAR_BORDER_WHITE 4

SamplerState sampler_states[5] : register(s0);
Texture2D texture_maps[4] : register(t0);
Texture2D metalSmoothTex : register(t4);

//���f���p�̃s�N�Z���V�F�[�_�[
PSOutPut main(VS_OUT pin)
{
    PSOutPut output;
    float4 albedoColor = texture_maps[0].Sample(sampler_states[ANISOTROPIC], pin.texcoord);
    output.Color = albedoColor;
    
    // �]�x�N�g��
    float3 N = normalize(pin.world_normal.xyz);
    // �ڃx�N�g�� 
    float3 T = normalize(pin.world_tangent.xyz);
    float sigma = pin.world_tangent.w;
    
    T = normalize(T - N * dot(N, T));
    float3 B = normalize(cross(N, T) * sigma);

    float4 normal = texture_maps[1].Sample(sampler_states[LINEAR], pin.texcoord);
    normal = (normal * 2.0) - 1.0;
    N = normalize((normal.x * normalize(T)) + (normal.y * normalize(B)) + (normal.z * normalize(N)));
    
    output.Normal = float4(N, 0);
    
    output.Position = pin.world_position;
    
    output.metalSmooth = metalSmoothTex.Sample(sampler_states[ANISOTROPIC], pin.texcoord);
    return output;
}