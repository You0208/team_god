#include "gltf_model.hlsli"
#include "gbuffer.hlsli"
#define BASECOLOR_TEXTURE 0
#define METALLIC_ROUGHNESS_TEXTURE 1
#define NORMAL_TEXTURE 2
#define EMISSIVE_TEXTURE 3
#define OCCLUSION_TEXTURE 4

Texture2D<float4> material_textures[5] : register(t1);
#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2

SamplerState sampler_states[3] : register(s0);

// �V���h�E�}�b�v
Texture2D shadow_map : register(t10);
SamplerState shadow_sampler_state : register(s10);

// IBL
TextureCube diffuse_iem : register(t33);
TextureCube specular_pmrem : register(t34);
Texture2D lut_ggx : register(t35);

PSGBufferOut main(VS_OUT pin, bool is_front_face : SV_IsFrontFace)
{
    material_constants m = materials[material];
    
    // �K���}�W��
    static const float GammaFactor = 2.2;
    
    // �x�[�X�J���[���擾
    float4 base_color = m.pbr_metallic_roughness.basecolor_factor;
    if (m.pbr_metallic_roughness.basecolor_texture.index > -1)
    {
        float4 sampled = material_textures[BASECOLOR_TEXTURE].Sample(sampler_states[ANISOTROPIC], pin.texcoord);
        sampled.rgb = pow(sampled.rgb, GammaFactor);
        base_color *= sampled;
    }
    clip(base_color.a - 0.25f);
    
    {
    // �f�B�U�p�^�[��
        static const int dither_pattern[16] =
        {
            0, 8, 2, 10,
            12, 4, 14, 6,
            3, 11, 1, 9,
            15, 7, 13, 5
        };
        int x = (int) pin.position.x % 4;
        int y = (int) pin.position.y % 4;
        float dither = (float) dither_pattern[x + y * 4] / 16.0f;
        clip(base_color.a - dither);
    }
    
    // ���Ȕ����F���擾
    float3 emisive_color = m.emissive_factor;
    if (m.emissive_texture.index > -1)
    {
        float3 emmisive = material_textures[EMISSIVE_TEXTURE].Sample(sampler_states[ANISOTROPIC], pin.texcoord).rgb;
        emmisive.rgb = pow(emmisive.rgb, GammaFactor);
        emisive_color.rgb *= emmisive.rgb;
    }
    
    // �@��/�]�@��/�ڐ�
    float3 N = normalize(pin.w_normal.xyz);
    float3 T = has_tangent ? normalize(pin.w_tangent.xyz) : float3(1, 0, 0);
    float sigma = has_tangent ? pin.w_tangent.w : 1.0;
    T = normalize(T - N * dot(N, T));
    float3 B = normalize(cross(N, T) * sigma);
    // ���ʕ`��̏ꍇ�͔��]���Ă���
    if (is_front_face == false)
    {
        T = -T;
        B = -B;
        N = -N;
    }
    
    // �@���}�b�s���O
    if (m.normal_texture.index > -1)
    {
        float4 sampled = material_textures[NORMAL_TEXTURE].Sample(sampler_states[LINEAR], pin.texcoord);
        float3 normal_factor = sampled.xyz;
        normal_factor = (normal_factor * 2.0f) - 1.0f;
        normal_factor = normalize(normal_factor * float3(m.normal_texture.scale, m.normal_texture.scale, 1.0f));
        N = normalize((normal_factor.x * T) + (normal_factor.y * B) + (normal_factor.z * N));
    }
    
    // �����x�N�g��
    float3 V = normalize(pin.w_position.xyz - camera_position.xyz);
    
    // ������/�e�����擾
    float roughness = m.pbr_metallic_roughness.roughness_factor;
    float metalness = m.pbr_metallic_roughness.metallic_factor;
    
    if (m.pbr_metallic_roughness.metallic_roughness_texture.index > -1)
    {
        float4 sampled = material_textures[METALLIC_ROUGHNESS_TEXTURE].Sample(sampler_states[LINEAR], pin.texcoord);
        roughness *= sampled.g;
        metalness *= sampled.b;
    }
    
    
    // ���̎Օ��l���擾
    float occlusion_factor = 1.0f;
    if (m.occlusion_texture.index > -1)
    {
        float4 sampled = material_textures[OCCLUSION_TEXTURE].Sample(sampler_states[LINEAR], pin.texcoord);
        occlusion_factor *= sampled.r;
    }
    const float occlusion_strength = m.occlusion_texture.strength;
    
    // GBufferData�ɏo�͏����܂Ƃ߂�
    GBufferData data;
    data.base_color = base_color;
    data.emissive_color = emisive_color;
    data.w_normal = N;
    data.w_position = pin.w_position.xyz;
    // data.depth ��Encode���͂���Ȃ�
    data.roughness = roughness;
    data.metalness = metalness;
    data.occlusion_factor = occlusion_factor;
    data.occlusion_strength = occlusion_strength;
    
    
    return EncodeGBuffer(data, view_projection);
}