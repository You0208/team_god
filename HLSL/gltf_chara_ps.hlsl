#include "gltf_model_my.hlsli"
#include "bidirectional_reflectance_distribution_function.hlsli"

#define BASECOLOR_TEXTURE 0
#define METALLIC_ROUGHNESS_TEXTURE 1
#define NORMAL_TEXTURE 2
#define EMISSIVE_TEXTURE 3
#define OCCLUSION_TEXTURE 4

Texture2D<float4> material_textures[5] : register(t1);


Texture2D noise_map : register(t9); // �搶�m�C�Y��3D

// SHADOW
SamplerComparisonState comparison_sampler_state : register(s5);
Texture2D shadow_map : register(t8);


float4 main(VS_OUT pin) : SV_TARGET
{
	// �K���}�W��
    static const float GAMMA = 2.2f;

//-----------------------------------------
// �T���v�����O
//-----------------------------------------
    // �F
    material_constants m = materials[material];
    
    float4 basecolor_factor = m.pbr_metallic_roughness.basecolor_factor;
    const int basecolor_texture = m.pbr_metallic_roughness.basecolor_texture.index;
    if (basecolor_texture > -1)
    {
        float4 sampled = material_textures[BASECOLOR_TEXTURE].Sample(sampler_states[ANISOTROPIC], pin.texcoord);
        sampled.rgb = pow(sampled.rgb, GAMMA);
        basecolor_factor *= sampled;
       // return sampled;
    }
    //return material_textures[BASECOLOR_TEXTURE].Sample(sampler_states[ANISOTROPIC], pin.texcoord);
    
    // �@���}�b�v
    float3 normal = material_textures[NORMAL_TEXTURE].Sample(sampler_states[LINEAR], pin.texcoord);
    
    float roughness_factor = m.pbr_metallic_roughness.roughness_factor;
    float metallic_factor = m.pbr_metallic_roughness.metallic_factor;
    const int metallic_roughness_texture = m.pbr_metallic_roughness.metallic_roughness_texture.index;
    if (metallic_roughness_texture > -1)
    {
        float4 sampled = material_textures[METALLIC_ROUGHNESS_TEXTURE].Sample(sampler_states[LINEAR], pin.texcoord);
        roughness_factor *= sampled.g;
        metallic_factor *= sampled.b;
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
    
    float occlusion_factor = 1.0;
    const int occlusion_texture = m.occlusion_texture.index;
    if (occlusion_texture > -1)
    {
        float4 sampled = material_textures[OCCLUSION_TEXTURE].Sample(sampler_states[LINEAR], pin.texcoord);
        occlusion_factor *= sampled.r;
    }
    const float occlusion_strength = m.occlusion_texture.strength;
    
    
    // �}�X�N
    float mask = noise_map.Sample(sampler_states[ANISOTROPIC], pin.texcoord).x;
    
//-----------------------------------------
//�@���C�e�B���O
//-----------------------------------------   
    
    float3 finalLig = 0;
    {
        // ���ˌ��̂����g�U���˂ɂȂ銄��
        float3 diffuseReflectance = lerp(basecolor_factor.rgb, 0.02f, metallic_factor);
       
        // �������ˎ��̃t���l�����˗�
        float3 F0 = lerp(0.04, basecolor_factor.rgb, metallic_factor);
        // �@��
        float3 N = normalize(pin.w_normal.xyz);
        // �ڐ��x�N�g��
        float3 T = has_tangent ? normalize(pin.w_tangent.xyz) : float3(1, 0, 0);
        float sigma = has_tangent ? pin.w_tangent.w : 1.0;
        T = normalize(T - N * dot(N, T));
        // �]�x�N�g��
        float3 B = normalize(cross(N, T) * sigma);

        const int normal_texture = m.normal_texture.index;
        if (normal_texture > -1)
        {
            float4 sampled = material_textures[NORMAL_TEXTURE].Sample(sampler_states[LINEAR], pin.texcoord);
            float3 normal_factor = sampled.xyz;
            normal_factor = (normal_factor * 2.0) - 1.0;
            normal_factor = normalize(normal_factor * float3(m.normal_texture.scale, m.normal_texture.scale, 1.0));
            N = normalize((normal_factor.x * T) + (normal_factor.y * B) + (normal_factor.z * N));
        }
        
	    // �����x�N�g��
        float3 V = normalize(pin.w_position.xyz - camera_position.xyz);
	
        //-----------------------------------------
        // ���ڌ���PBR
        //-----------------------------------------   
        float3 directDiffuse = 0, directSpecular = 0;
        float3 L = normalize(directional_light_direction.xyz);
        DirectBDRF(diffuseReflectance, F0, N, V, L,
			directional_light_color.rgb, roughness_factor, directDiffuse, directSpecular);
        // �ŏI���ɑ������킹��
        finalLig += (directDiffuse + directSpecular);
        //-----------------------------------------
        // �|�C���g���C�g��PBR
        //-----------------------------------------  
        float3 pointDiffuse = 0, pointSpecular = 0;
        PointLight(pin, diffuseReflectance, F0, N, V, roughness_factor, pointDiffuse, pointSpecular);
        // �ŏI���ɑ������킹�� 
        finalLig += (pointDiffuse + pointSpecular);
        
        //-----------------------------------------
        // �X�|�b�g���C�g��PBR
        //-----------------------------------------  
        float3 spotDiffuse = 0, spotSpecular = 0;
        SpotLight(pin, diffuseReflectance, F0, N, V, roughness_factor, spotDiffuse, spotSpecular);
        // �ŏI���ɑ������킹��         
        finalLig += (spotDiffuse + spotSpecular);
        
        //-----------------------------------------
        //�@�V���h�E
        //----------------------------------------- 
    
        float3 shadow_factor = 1.0f;
        Shadow(pin, normal, T, B, shadow_map, comparison_sampler_state, shadow_factor);
        // �ŏI���ɑ������킹��    
        finalLig *= shadow_factor;
        
        // �G�~�b�V�u
        finalLig += emmisive_factor;
        
        // �I�N���[�W����
        finalLig = lerp(finalLig, finalLig * occlusion_factor, 1.0f);
        //-----------------------------------------
        //�@�������C�g
        //----------------------------------------- 
        //float3 rimColor = 0, hemiLight = 0;
        //LimHemiLight(pin, normal, N_, T, E, rimColor, hemiLight);
        //// �ŏI���ɑ������킹��     
        //finalLig += (rimColor + hemiLight);
    }
 
//-----------------------------------------
//�@�f�B�]���u
//----------------------------------------- 
    //{
    //    float dissolve = step(mask, threshold);
    //    color.a *= dissolve;
    //    // �A���t�@��0�ȉ��Ȃ炻�������`�悵�Ȃ��悤�ɂ���
    //    clip(color.a - 0.01f);
    //}
    
    
    finalLig.rgb = pow(finalLig.rgb, 1.0f / GAMMA);
    return float4(finalLig, basecolor_factor.a);
};