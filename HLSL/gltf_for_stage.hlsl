#include "gltf_model_re.hlsli"

#define BASECOLOR_TEXTURE 0
#define METALLIC_ROUGHNESS_TEXTURE 1
#define NORMAL_TEXTURE 2
#define EMISSIVE_TEXTURE 3
#define OCCLUSION_TEXTURE 4

Texture2D<float4> material_textures[5] : register(t1);

// SHADOW
SamplerComparisonState comparison_sampler_state : register(s5);
Texture2D shadow_map : register(t8);

float4 main(VS_OUT pin) : SV_TARGET
{
//-----------------------------------------
// �T���v�����O
//-----------------------------------------
    const float GAMMA = 2.2;
    
    material_constants m = materials[material];
    
    // �x�[�X�J���[
    float4 basecolor_factor = m.pbr_metallic_roughness.basecolor_factor;
    const int basecolor_texture = m.pbr_metallic_roughness.basecolor_texture.index;
    if (basecolor_texture > -1)
    {
        float4 sampled = material_textures[BASECOLOR_TEXTURE].Sample(sampler_states[ANISOTROPIC], pin.texcoord);
        sampled.rgb = pow(sampled.rgb, GAMMA);
        basecolor_factor *= sampled;
    }
    
//-----------------------------------------
// ���C�e�B���O
//-----------------------------------------  
    float3 finalLig = 0;
    float3 shadow_factor = 1.0f;
    {    
        // �@���x�N�g��
        float3 N_ = normalize(pin.w_normal.xyz);
        return (N_,1);
        // �ڐ��x�N�g��
        float3 T = has_tangent ? normalize(pin.w_tangent.xyz) : float3(1, 0, 0);
        float sigma = has_tangent ? pin.w_tangent.w : 1.0;
        // �ڃx�N�g���͖@���}�b�v����v�Z����A�@���x�N�g�� N �ƒ�������悤�ɒ���
        T = normalize(T - N_ * dot(N_, T));
        // �]�x�N�g��
        float3 B = normalize(cross(N_, T) * sigma);
        //-----------------------------------------
        // ���ڌ�
        //-----------------------------------------  
        float3 diffuse = 0, specular = 0;
        CalcLambertDiffuse(directional_light_direction.xyz, directional_light_color.xyz, pin.w_normal.xyz);
        // �ŏI���ɑ������킹��
        finalLig += diffuse + specular;
        
#if 0
    diffuse += ibl_radiance_lambertian(N, V, roughness_factor, c_diff, f0);
    specular += ibl_radiance_ggx(N, V, roughness_factor, f0);
#endif
        //-----------------------------------------
        // �|�C���g���C�g
        //-----------------------------------------  
        float3 point_diffuse = 0, point_specular = 0;
    
        // �ŏI���ɑ������킹��
        finalLig += point_diffuse + point_specular;
    
        //-----------------------------------------
        // �X�|�b�g���C�g
        //----------------------------------------- 
        float3 spot_diffuse = 0, spot_specular = 0;
        
        // �ŏI���ɑ������킹��
        finalLig += spot_diffuse + spot_specular;
     
        //-----------------------------------------
        // �V���h�E
        //-----------------------------------------   
        Shadow(pin, N_, T, B, shadow_map, comparison_sampler_state, shadow_factor);
        
    }
    
    // �ŏI���ɑ������킹��    
    finalLig *= shadow_factor;
    
    return float4(finalLig, basecolor_factor.a);
}
