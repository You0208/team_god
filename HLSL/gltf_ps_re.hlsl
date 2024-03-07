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
    
    // �G�~�b�V�u
    float3 emmisive_factor = m.emissive_factor;
    const int emissive_texture = m.emissive_texture.index;
    if (emissive_texture > -1)
    {
        float4 sampled = material_textures[EMISSIVE_TEXTURE].Sample(sampler_states[ANISOTROPIC], pin.texcoord);
        sampled.rgb = pow(sampled.rgb, GAMMA);
        emmisive_factor *= sampled.rgb;
    }
    // ���t�l�X�E���^���l�X
    float roughness_factor = m.pbr_metallic_roughness.roughness_factor;
    float metallic_factor = m.pbr_metallic_roughness.metallic_factor;
    const int metallic_roughness_texture = m.pbr_metallic_roughness.metallic_roughness_texture.index;
    if (metallic_roughness_texture > -1)
    {
        float4 sampled = material_textures[METALLIC_ROUGHNESS_TEXTURE].Sample(sampler_states[LINEAR], pin.texcoord);
        roughness_factor *= sampled.g;
        metallic_factor *= sampled.b;
    }
    
    // �I�N���[�W����
    float occlusion_factor = 1.0;
    const int occlusion_texture = m.occlusion_texture.index;
    if (occlusion_texture > -1)
    {
        float4 sampled = material_textures[OCCLUSION_TEXTURE].Sample(sampler_states[LINEAR], pin.texcoord);
        occlusion_factor *= sampled.r;
    }
    const float occlusion_strength = m.occlusion_texture.strength;
     
    // �@���}�b�v
    const int normal_texture = m.normal_texture.index;
    float3 normal_factor = 0;
    if (normal_texture > -1)
    {
        float4 sampled = material_textures[NORMAL_TEXTURE].Sample(sampler_states[LINEAR], pin.texcoord);
        normal_factor = sampled.xyz;
        normal_factor = (normal_factor * 2.0) - 1.0;
        normal_factor = normalize(normal_factor * float3(m.normal_texture.scale, m.normal_texture.scale, 1.0));
    }
//-----------------------------------------
// ���C�e�B���O
//-----------------------------------------  
    float3 finalLig = 0;
    float3 shadow_factor = 1.0f;
    {    
        // �������ˎ��̃t���l�����˗��i���̂̋������Ɗ�{�F�Ɋ�Â��āA���̂̕\�ʂ̌��̔��˗��𐄒�j
        const float3 f0 = lerp(0.04, basecolor_factor.rgb, metallic_factor);
        const float3 f90 = 1.0;
        // �e����2�悷�邱�Ƃɂ��A���̂̕\�ʂ̔��ׂȕϓ�����苭�������
        const float alpha_roughness = roughness_factor * roughness_factor;
        // ���^���b�N�̒l�ɉ����ăx�[�X�J���[�̉e���𒲐�����
        const float3 c_diff = lerp(basecolor_factor.rgb, 0.0, metallic_factor);

        // �ʒu
        const float3 P = pin.w_position.xyz;
        // �����ւ̃x�N�g��
        const float3 V = normalize(camera_position.xyz - pin.w_position.xyz);
        // �@���x�N�g��
        float3 N_ = normalize(pin.w_normal.xyz);
        // �ڐ��x�N�g��
        float3 T = has_tangent ? normalize(pin.w_tangent.xyz) : float3(1, 0, 0);
        float sigma = has_tangent ? pin.w_tangent.w : 1.0;
        // �ڃx�N�g���͖@���}�b�v����v�Z����A�@���x�N�g�� N �ƒ�������悤�ɒ���
        T = normalize(T - N_ * dot(N_, T));
        // �]�x�N�g��
        float3 B = normalize(cross(N_, T) * sigma);
        // ���[���h��Ԃł̖@���x�N�g���𓾂�   
        float3 N = normalize((normal_factor.x * T) + (normal_factor.y * B) + (normal_factor.z * N_));
        // ���C�g�̕���
        float3 L = normalize(-directional_light_direction.xyz);
        // ���C�g�̐F
        float3 Li = directional_light_color;
        
        //-----------------------------------------
        // ���ڌ���PBR
        //-----------------------------------------  
        float3 diffuse = 0, specular = 0;

        // �@���x�N�g���ƌ����ւ̃x�N�g���Ƃ̓���
        const float NoL = max(0.0, dot(N, L));
        // �@���x�N�g���Ǝ����ւ̃x�N�g���Ƃ̓���
        const float NoV = max(0.0, dot(N, V));
        
        // ���ς�0����Ȃ��Ȃ�i�����łȂ��Ȃ�j
        if (NoL > 0.0 || NoV > 0.0)
        {
            const float3 R = reflect(-L, N); // ���˃x�N�g��
            const float3 H = normalize(V + L); // �n�[�t�x�N�g���i�����ւ̃x�N�g���Ǝ��_�ւ̃x�N�g���̒��ԃx�N�g���j
        
            const float NoH = max(0.0, dot(N, H)); // �@���x�N�g���ƃn�[�t�x�N�g���̓���
            const float HoV = max(0.0, dot(H, V)); // VoH: �����x�N�g���ƃn�[�t�x�N�g���̓���
        
            diffuse += Li * NoL * brdf_lambertian(f0, f90, c_diff, HoV);
            specular += Li * NoL * brdf_specular_ggx(f0, f90, alpha_roughness, HoV, NoL, NoV, NoH);
        }
        
        // �ŏI���ɑ������킹��
        finalLig += diffuse + specular;
#if 0
    diffuse += ibl_radiance_lambertian(N, V, roughness_factor, c_diff, f0);
    specular += ibl_radiance_ggx(N, V, roughness_factor, f0);
#endif
        //-----------------------------------------
        // �|�C���g���C�g��PBR
        //-----------------------------------------  
        float3 point_diffuse = 0, point_specular = 0;
        PointLight(pin, N, V, f0, f90, alpha_roughness, c_diff, point_diffuse, point_specular);
    
        // �ŏI���ɑ������킹��
        finalLig += point_diffuse + point_specular;
    
        //-----------------------------------------
        // �X�|�b�g���C�g��PBR
        //----------------------------------------- 
        float3 spot_diffuse = 0, spot_specular = 0;
        SpotLight(pin, N, V, f0, f90, alpha_roughness, c_diff, spot_diffuse, spot_specular);
        
        // �ŏI���ɑ������킹��
        finalLig += spot_diffuse + spot_specular;
     
        //-----------------------------------------
        // �V���h�E
        //-----------------------------------------   
        Shadow(pin, normal_factor, T, B, shadow_map, comparison_sampler_state, shadow_factor);
        
    }
    
    //  �G�~�b�V�u
    float3 emmisive = emmisive_factor;
    finalLig += emmisive_factor;
    //  �I�N���[�W����
    finalLig = lerp(finalLig, finalLig * occlusion_factor, occlusion_strength);

    // �ŏI���ɑ������킹��    
    finalLig *= shadow_factor;
    
    return float4(finalLig, basecolor_factor.a);
}
