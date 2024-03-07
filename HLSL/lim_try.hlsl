#include "lim_try.hlsli"

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
#define LINEAR_BORDER_BLACK 3
#define LINEAR_BORDER_WHITE 4

SamplerState sampler_states[3] : register(s0);
Texture2D texture_maps[8] : register(t0);

Texture2D noise_map : register(t9); // �搶�m�C�Y��3D

Texture2D roughness_ : register(t4);
Texture2D metalness_ : register(t5);

// SHADOW
SamplerComparisonState comparison_sampler_state : register(s5);
Texture2D shadow_map : register(t8);

///////////////////////////////////////////
// �V�F�[�_�[���\�[�X
///////////////////////////////////////////
// ���f���e�N�X�`��

///////////////////////////////////////////
// �T���v���[�X�e�[�g
///////////////////////////////////////////
sampler g_sampler : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 diffuse_color = texture_maps[0].Sample(sampler_states[ANISOTROPIC], pin.texcoord);
    
    // ���������x�N�g��
    float3 E = normalize(pin.world_position.xyz - camera_position.xyz);
    // ���C�g�����x�N�g��
    float3 L = normalize(directional_light_direction.xyz);
    // �e�x�N�g���𐳋K�����A�������s��̊e��Ɋ��蓖��
    float3x3 mat = { normalize(pin.world_tangent.xyz), normalize(pin.binormal), normalize(pin.world_normal.xyz) };
    // �@���}�b�v�̃T���v�����O
    float3 N = texture_maps[1].Sample(sampler_states[ANISOTROPIC], pin.texcoord).rgb;
    // �m�[�}���e�N�X�`���@�������[���h�֕ϊ�
    N = normalize(mul(N * 2.0f - 1.0f, mat));
    
    // ���s�������擾
    float3 directional_diffuse = CalcLambert(N, L, directional_light_color.rgb, float3(1, 1, 1));

    float3 ambient = CalcHemiSphereLight(N, float3(0, 1, 0), sky_color.rgb, ground_color.rgb, hemisphere_weight);
    
    float4 color = float4(ambient.rgb, diffuse_color.a);
    color.rgb += diffuse_color.rgb * (directional_diffuse);
    
    float3 rim_color = CalcRimLight(N, E, L, directional_light_color.rgb);
    color.rgb += rim_color;
    
    return color;
}