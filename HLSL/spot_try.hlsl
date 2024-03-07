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

///////////////////////////////////////////
// �֐��錾
///////////////////////////////////////////
float3 CalcLigFromDirectionLight(VS_OUT pin);

float4 main(VS_OUT pin) : SV_TARGET
{
    // �f�B���N�V�������C�g�ɂ�郉�C�e�B���O���v�Z����
    float3 directionLig = CalcLigFromDirectionLight(pin);

    // step-6 �T�[�t�F�C�X�ɓ��˂���X�|�b�g���C�g�̌��̌������v�Z����
    // �s�N�Z���̍��W - �X�|�b�g���C�g�̍��W���v�Z
    float3 ligDir = pin.world_position - spot_light[0].position;

    // ���K�����đ傫��1�̃x�N�g���ɂ���
    ligDir = normalize(ligDir);
    
    // step-7 �����Ȃ���Lambert�g�U���ˌ����v�Z����
    float3 diffSpotLight = CalcLambertDiffuse(
        ligDir, // ���C�g�̕���
        spot_light[0].color.rgb, // ���C�g�̃J���[
        pin.world_normal.rgb // �T�[�t�F�C�X�̖@��
    );
    // step-8 �����Ȃ���Phong���ʔ��ˌ����v�Z����
    float3 specSpotLight = CalcPhongSpecular(
        ligDir, // ���C�g�̕���
        spot_light[0].color.rgb, // ���C�g�̃J���[
        pin.world_position.rgb, // �T�[�t�F�C�X�̃��[���h���W
        pin.world_normal.rgb     // �T�[�t�F�C�X�̖@��
    );
    
    // step-9 �����ɂ��e�������v�Z����
    // �X�|�b�g���C�g�Ƃ̋������v�Z����
    float3 distance = length(pin.world_position.rgb - spot_light[0].position.rgb);
    
    // �e�����͋����ɔ�Ⴕ�ď������Ȃ��Ă���
    float affect = 1.0f - 1.0f / spot_light[0].range* distance;
       // �e���͂��}�C�i�X�ɂȂ�Ȃ��悤�ɕ␳��������
    if (affect < 0.0f)
    {
        affect = 0.0f;
    }
    // �e���̎d�����w���֐��I�ɂ���B����̃T���v���ł�3�悵�Ă���
    affect = pow(affect, 6.0f);
    
    // step-10 �e��������Z���Ĕ��ˌ�����߂�
    diffSpotLight *= affect;
    specSpotLight *= affect;
    
    // step-11 ���ˌ��Ǝˏo�����̊p�x�����߂�
        // dot()�𗘗p���ē��ς����߂�
    float angle = dot(ligDir, spot_light[0].direction.xyz);
    
    // dot()�ŋ��߂��l��acos()�ɓn���Ċp�x�����߂�
    angle = abs(acos(angle));
    // step-12 �p�x�ɂ��e���������߂�
    // �p�x�ɔ�Ⴕ�ď������Ȃ��Ă����e�������v�Z����
    affect = 1.0f - 1.0f / spot_light[0].angle* angle;
        // �e�������}�C�i�X�ɂȂ�Ȃ��悤�ɕ␳��������
    if (affect < 0.0f)
    {
        affect = 0.0f;
    }
    // �e���̎d�����w���֐��I�ɂ���B����̃T���v���ł�0.5�悵�Ă���
    affect = pow(affect, 0.5f);
    // step-13 �p�x�ɂ��e�����𔽎ˌ��ɏ�Z���āA�e������߂�
    diffSpotLight *= affect;
    specSpotLight *= affect;
    // �f�B���N�V�������C�g+�|�C���g���C�g+���������߂�
    float3 finalLig = directionLig
                    + ambient_color.xyz;
    
    // step-14 �X�|�b�g���C�g�̔��ˌ����ŏI�I�Ȕ��ˌ��ɑ����Z����
    finalLig += diffSpotLight + specSpotLight;
    
    // �f�B�t���[�Y�}�b�v���T���v�����O
    float4 finalColor = texture_maps[0].Sample(sampler_states[ANISOTROPIC], pin.texcoord);

    // �e�N�X�`���J���[�ɋ��߂�������Z���čŏI�o�̓J���[�����߂�
    finalColor.xyz *= finalLig;
    
    return finalColor;
}

/// <summary>
/// </summary
/// �f�B���N�V�������C�g�ɂ�锽�ˌ����v�Z
/// <param name="psIn">�s�N�Z���V�F�[�_�[����̓��́B</param>
float3 CalcLigFromDirectionLight(VS_OUT pin)
{
    // �f�B���N�V�������C�g�ɂ��Lambert�g�U���ˌ����v�Z����
    float3 diffDirection = CalcLambertDiffuse(directional_light_direction.xyz, directional_light_color.xyz, pin.world_normal.rgb);

    // �f�B���N�V�������C�g�ɂ��Phong���ʔ��ˌ����v�Z����
    float3 specDirection = CalcPhongSpecular(
            directional_light_direction.xyz, directional_light_color.xyz, pin.world_position.rgb, pin.world_normal.rgb);
    return diffDirection + specDirection;
}