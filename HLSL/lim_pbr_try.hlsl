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
    // �T���v�����O
    float4 color = texture_maps[0].Sample(sampler_states[ANISOTROPIC], pin.texcoord);
    float alpha = color.a;
    
    // �����x
    float metallic = metalness_.Sample(sampler_states[ANISOTROPIC], pin.texcoord);
    
    // ���炩��
    float smoothness = roughness_.Sample(sampler_states[ANISOTROPIC], pin.texcoord);
    // �e��
    float roughness = 1.0f - smoothness;
#if 0
    // ����
    metallic = saturate(metallic + adjustMetalness);
    smoothness = saturate(smoothness + adjustSmoothness);
 #endif
    
     //FOG
    {
        color = CalcFog(color, dis_fog_color, dis_fog_range.xy, length(pin.world_position.xyz - camera_position.xyz));
    }
    
    // ���ˌ��̂����g�U���˂ɂȂ銄��
    float3 diffuseReflectance = lerp(color.rgb, 0.02f, metallic);
   // �������ˎ��̃t���l�����˗�
    float3 F0 = lerp(Dielectric, color.rgb, metallic);
    
    // �@��
    float3 N_ = normalize(pin.world_normal.xyz);
    // �ڃx�N�g��
    float3 T = normalize(pin.world_tangent.xyz);
    float sigma = pin.world_tangent.w;
    // �ڃx�N�g���͖@���}�b�v����v�Z����A�@���x�N�g�� N �ƒ�������悤�ɒ���
    T = normalize(T - N_ * dot(N_, T));
    // �]�x�N�g��
    float3 B = normalize(cross(N_, T) * sigma);
    
    // �@���}�b�v����xyz�������擾����( -1 �` +1 )�̊ԂɃX�P�[�����O
    float3 normal = texture_maps[1].Sample(sampler_states[LINEAR], pin.texcoord).xyz * 2.0f - 1.0f;
    float3 N = normalize(normal.x * normalize(T) + normal.y * normalize(B) + normal.z * normalize(N_));
    float3 E = normalize(camera_position.xyz - pin.world_position.xyz);
    // ���C�e�B���O�v�Z
    
    float3 directDiffuse = 0, directSpecular = 0;
    
    // ���s�����̏���
    DirectBDRF(diffuseReflectance, F0, N, E, directional_light_direction.xyz,
         directional_light_color.rgb, roughness,
         directDiffuse, directSpecular);
    
    
#if 1
    // �X�|�b�g���C�g�̏����iPBR�����j
    for (int j = 0; j < 1; ++j)
    {
        // �T�[�t�F�C�X�ɓ��˂���X�|�b�g���C�g�̌��̌������v�Z����
        float3 ligDir = pin.world_position.xyz - spot_light[j].position.xyz;
        // �X�|�b�g���C�g�̃J���[
        float3 spColor = spot_light[j].color.xyz;
        // ���K�����đ傫��1�̃x�N�g���ɂ���
        ligDir = normalize(ligDir);
        
        // �����Ȃ���Lambert�g�U���ˌ����v�Z����
        float3 diffSpotLight = CalcLambertDiffuse(
        ligDir, // ���C�g�̕���
        spColor, // ���C�g�̃J���[
        pin.world_normal.rgb // �T�[�t�F�C�X�̖@��
        );
        // �����Ȃ���Phong���ʔ��ˌ����v�Z����
            float3 specSpotLight = CalcPhongSpecular(
            ligDir, // ���C�g�̕���
            spColor, // ���C�g�̃J���[
            pin.world_position.rgb, // �T�[�t�F�C�X�̃��[���h���W
            pin.world_normal.rgb     // �T�[�t�F�C�X�̖@��
        );
        
        
        // �����ɂ��e�������v�Z����
        // �X�|�b�g���C�g�Ƃ̋������v�Z����
        float3 distance = length(pin.world_position.xyz - spot_light[j].position.xyz);

        // �e�����͋����ɔ�Ⴕ�ď������Ȃ��Ă���
        float affect = 1.0f - 1.0f / spot_light[j].range * distance;
        
        // �e���͂��}�C�i�X�ɂȂ�Ȃ��悤�ɕ␳��������
        if (affect < 0.0f)
        {
            affect = 0.0f;
        }

        // �e���̎d�����w���֐��I�ɂ���
        affect = pow(affect, 6.0f);
        
        // �e��������Z���Ĕ��ˌ�����߂�
        diffSpotLight *= affect;
        specSpotLight *= affect;
        
        // ���ˌ��Ǝˏo�����̊p�x�����߂�
        // dot()�𗘗p���ē��ς����߂�
        float angle = dot(ligDir, spot_light[j].direction.xyz);
        
        // dot()�ŋ��߂��l��acos()�ɓn���Ċp�x�����߂�
        angle = abs(acos(angle));
        
        // �p�x�ɂ��e���������߂�
        // �p�x�ɔ�Ⴕ�ď������Ȃ��Ă����e�������v�Z����
        affect = 1.0f - 1.0f / spot_light[j].angle * angle;

        // �e�������}�C�i�X�ɂȂ�Ȃ��悤�ɕ␳��������
        if (affect < 0.0f)
        {
            affect = 0.0f;
        }
        
        // �e���̎d�����w���֐��I�ɂ���B����̃T���v���ł�0.5�悵�Ă���
        affect = pow(affect, 0.5f);

        // �p�x�ɂ��e�����𔽎ˌ��ɏ�Z���āA�e������߂�
        diffSpotLight *= affect;
        specSpotLight *= affect;
        
        directDiffuse += diffSpotLight;
        directSpecular += specSpotLight;
        
    }
#else

    // �_�����̏���
    for (int i = 0; i < 8; ++i)
    {
        // �T�[�t�F�C�X�ɓ��˂��郉�C�g�̌��̌������v�Z����
        float3 lightVector = pin.world_position.xyz - point_light[i].position.xyz;
        float lightLength = length(lightVector);
        if (lightLength >= point_light[i].range)
            continue;
        float attenuate = saturate(1.0f - lightLength / point_light[i].range);
        lightVector = lightVector / lightLength;
        
        // �|�C���g���C�g�Ƃ̋������v�Z����
        float3 distance = length(pin.world_position.xyz - point_light[i].position.xyz);
        // �e�����͋����ɔ�Ⴕ�ď������Ȃ��Ă���
        float affect = 1.0f - 1.0f / point_light[i].range.x * distance;
        // �e���͂��}�C�i�X�ɂȂ�Ȃ��悤�ɕ␳��������
        if (affect < 0.0f)
        {
            affect = 0.0f;
        }
        //�e�����w���֐��I�ɂ���B����̃T���v���ł�3�悵�Ă���
        affect = pow(affect, 3.0f);
        
        float3 diffuse = 0, specular = 0;
        DirectBDRF(diffuseReflectance, F0, N, E, lightVector,
        point_light[i].color.rgb, roughness,
        diffuse, specular);
        
        directDiffuse += diffuse * attenuate;
        directSpecular += specular * attenuate;
    }
    
    
#endif
    
    // �f�B�]���u
    {
        float mask = noise_map.Sample(sampler_states[ANISOTROPIC], pin.texcoord).x;
    
        // step�֐���p����mask�̒l��dissolveThreshold�̒l���r���ē��ߒl��0 or 1�ɂ���
        float dissolve = step(mask, threshold);
        // color�̓��ߒl�ɏ�Z����
        //color.a *= mask;
        color.a *= dissolve;
        // �A���t�@��0�ȉ��Ȃ炻�������`�悵�Ȃ��悤�ɂ���
        clip(color.a - 0.01f);
    }
    
    // SHADOW
#if 0
    const float shadow_depth_bias = 0.001;
#else
    float3 L = normalize(-directional_light_direction.xyz);
    float3 SN = normalize(pin.world_normal.xyz);
    float3 s_normal = texture_maps[1].Sample(sampler_states[LINEAR], pin.texcoord).xyz;
    SN = normalize((s_normal.x * T) + (s_normal.y * B) + (s_normal.z * SN));
	// Here we have a maximum bias of 0.01 and a minimum of 0.001 based on the surface's normal and light direction. 
    const float shadow_depth_bias = max(0.01 * (1.0 - dot(SN, L)), 0.001);
#endif

    float4 light_view_position = mul(pin.world_position, light_view_projection);
    light_view_position = light_view_position / light_view_position.w;
    float2 light_view_texcoord = 0;
    light_view_texcoord.x = light_view_position.x * +0.5 + 0.5;
    light_view_texcoord.y = light_view_position.y * -0.5 + 0.5;
    float depth = saturate(light_view_position.z - shadow_depth_bias);

    float3 shadow_factor = 1.0f;
    shadow_factor = shadow_map.SampleCmpLevelZero(comparison_sampler_state, light_view_texcoord, depth).xxx;
    

    float3 hemiLight = 0;
 // �������C�g�i���s�����ƂԂ��邽�ߗv�C���j
#if 0
    // ���C�g�����x�N�g��
    float3 Light = normalize(directional_light_direction.xyz);
    // �e�x�N�g���𐳋K�����A�������s��̊e��Ɋ��蓖��
   float3x3 mat = { T, pin.binormal, N_ };
    // �@���}�b�v�̃T���v�����O
    float3 RimN = texture_maps[1].Sample(sampler_states[ANISOTROPIC], pin.texcoord).rgb;
    // �m�[�}���e�N�X�`���@�������[���h�֕ϊ�
    RimN = normalize(mul(RimN * 2.0f - 1.0f, mat));
    
    float3 rimColor = CalcRimLight(RimN, -E, Light, directional_light_color.rgb);
    
    directDiffuse += rimColor;
    // �������C�g
    hemiLight = CalcHemiSphereLight(RimN, float3(0, 1, 0), sky_color.rgb, ground_color.rgb, hemisphere_weight);
    
#endif
    
    return float4((directDiffuse + directSpecular + hemiLight) * shadow_factor /*SHADOW*/, color.a);

}