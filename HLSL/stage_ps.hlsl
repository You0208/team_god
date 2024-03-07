#include "fog_pbr.hlsli"

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
#define LINEAR_BORDER_BLACK 3
#define LINEAR_BORDER_WHITE 4

SamplerState sampler_states[3] : register(s0);
Texture2D texture_maps[6] : register(t0);

Texture2D noise_map : register(t9); // �搶�m�C�Y��3D

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
    // �f�B�t���[�Y�}�b�v���T���v�����O
    float4 color = texture_maps[0].Sample(sampler_states[ANISOTROPIC], pin.texcoord);
    float alpha = color.a;
    
    // �����x
    float metallic = 0;
    // ���炩��
    float roughness = texture_maps[5].Sample(sampler_states[ANISOTROPIC], pin.texcoord).a;
    //// ����
    //metallic = saturate(metallic + adjustMetalness);
    //smoothness = saturate(smoothness + adjustSmoothness);
    // �e��
    //float roughness = 1.0f - smoothness;
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
    DirectBDRF(diffuseReflectance, F0, N, E, light_direction.xyz,
         dirColor.rgb, roughness,
         directDiffuse, directSpecular);
    // �_�����̏���
    {
        float3 lightVector = pin.world_position.xyz - ptPosition.xyz;
        float lightLength = length(lightVector);
        // �����������Z�o����
        float attenuate = saturate(1.0f - lightLength / ptRange.x);
        lightVector = lightVector / lightLength;
        // �����ɂ��e�������v�Z����
        // �|�C���g���C�g�Ƃ̋������v�Z����
        float3 distance = length(pin.world_position.xyz - ptPosition.xyz);
        // �e�����͋����ɔ�Ⴕ�ď������Ȃ��Ă���
        float affect = 1.0f - 1.0f / ptRange.x * distance;
        // �e���͂��}�C�i�X�ɂȂ�Ȃ��悤�ɕ␳��������
        if (affect < 0.0f)
        {
            affect = 0.0f;
        }
        //�e�����w���֐��I�ɂ���B����̃T���v���ł�3�悵�Ă���
        affect = pow(affect, 3.0f);
        float3 diffuse = 0, specular = 0;
        DirectBDRF(diffuseReflectance, F0, N, E, lightVector,
        ptColor.rgb, roughness,
        diffuse, specular);
        directDiffuse += diffuse * attenuate;
        directSpecular += specular * attenuate;
    }
    
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
    float3 L = normalize(-light_direction.xyz);
	// Here we have a maximum bias of 0.01 and a minimum of 0.001 based on the surface's normal and light direction. 
    const float shadow_depth_bias = max(0.01 * (1.0 - dot(N, L)), 0.001);
#endif

    float4 light_view_position = mul(pin.world_position, light_view_projection);
    light_view_position = light_view_position / light_view_position.w;
    float2 light_view_texcoord = 0;
    light_view_texcoord.x = light_view_position.x * +0.5 + 0.5;
    light_view_texcoord.y = light_view_position.y * -0.5 + 0.5;
    float depth = saturate(light_view_position.z - shadow_depth_bias);

    float3 shadow_factor = 1.0f;
    shadow_factor = shadow_map.SampleCmpLevelZero(comparison_sampler_state, light_view_texcoord, depth).xxx;
    
	// EMISSIVE
    float3 emissive = texture_maps[2].Sample(sampler_states[LINEAR], pin.texcoord).rgb;


    const float emissive_intensity = 3.0;

    return float4((directDiffuse + directSpecular + emissive * emissive_intensity) * shadow_factor /*SHADOW*/, color.a);

}