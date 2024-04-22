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
    static const float GammaFactor = 2.2f;

//-----------------------------------------
// �T���v�����O
//-----------------------------------------
    // �F
    float4 color = material_textures[BASECOLOR_TEXTURE].Sample(sampler_states[ANISOTROPIC], pin.texcoord);
    float alpha = color.a;
    color.rgb = pow(color.rgb, GammaFactor);
    // �����x
    float metallic = material_textures[METALLIC_ROUGHNESS_TEXTURE].Sample(sampler_states[LINEAR], pin.texcoord).b;
    
    //// ���炩��
    //float smoothness = roughness_.Sample(sampler_states[ANISOTROPIC], pin.texcoord);
   
    // �@���}�b�v
    float3 normal = material_textures[NORMAL_TEXTURE].Sample(sampler_states[LINEAR], pin.texcoord);
    
    // �e��
    float roughness = material_textures[METALLIC_ROUGHNESS_TEXTURE].Sample(sampler_states[LINEAR], pin.texcoord).g;
    
    // �}�X�N
    float mask = noise_map.Sample(sampler_states[ANISOTROPIC], pin.texcoord).x;
    
    // �G�~�b�V�u
    float4 emmisive = material_textures[EMISSIVE_TEXTURE].Sample(sampler_states[ANISOTROPIC], pin.texcoord);
    
    // �I�N���[�W����
    float4 occlusion = material_textures[OCCLUSION_TEXTURE].Sample(sampler_states[LINEAR], pin.texcoord);
    float occlusion_factor = 1.0f;
    occlusion_factor *= occlusion;
    
//-----------------------------------------
//�@���C�e�B���O
//-----------------------------------------   
    
    float3 finalLig = 0;
    {
        // ���ˌ��̂����g�U���˂ɂȂ銄��
        float3 diffuseReflectance = lerp(color.rgb, 0.02f, metallic);
       
        // �������ˎ��̃t���l�����˗�
        float3 F0 = lerp(0.04, color.rgb, metallic);
        // �@��
        float3 N_ = normalize(pin.w_normal.xyz);
        // �ڐ��x�N�g��
        float3 T = has_tangent ? normalize(pin.w_tangent.xyz) : float3(1, 0, 0);
        float sigma = has_tangent ? pin.w_tangent.w : 1.0;
        T = normalize(T - N_ * dot(N_, T));
        // �]�x�N�g��
        float3 B = normalize(cross(N_, T) * sigma);
        // �@���}�b�v����xyz�������擾����( -1 �` +1 )�̊ԂɃX�P�[�����O
        float3 normal_ = normal * 2.0f - 1.0f;
        // ���[���h��Ԃł̖@���x�N�g���𓾂�
        float3 N = normalize(normal_.x * normalize(T) + normal_.y * normalize(B) + normal_.z * normalize(N_));
        // �J�������璸�_�ւ̕����x�N�g��
        float3 E = normalize(camera_position.xyz - pin.w_position.xyz);
  
	    // �����x�N�g��
        float3 V = normalize(pin.w_position.xyz - camera_position.xyz);
	
        
        float NdotV = max(0.0001f, dot(N, E));
        //return float4(NdotV, 0, 0, 1);
        
        //-----------------------------------------
        // ���ڌ���PBR
        //-----------------------------------------   
        float3 directDiffuse = 0, directSpecular = 0;
        float3 L = normalize(directional_light_direction.xyz);
        DirectBDRF(diffuseReflectance, F0, N, V, L,
			directional_light_color.rgb, roughness, directDiffuse, directSpecular);
        // �ŏI���ɑ������킹��
        finalLig += (directDiffuse + directSpecular);
        //-----------------------------------------
        // �|�C���g���C�g��PBR
        //-----------------------------------------  
        float3 pointDiffuse = 0, pointSpecular = 0;
        PointLight(pin, diffuseReflectance, F0, N, V, roughness, pointDiffuse, pointSpecular);
        // �ŏI���ɑ������킹�� 
        finalLig += (pointDiffuse + pointSpecular);
        
        //-----------------------------------------
        // �X�|�b�g���C�g��PBR
        //-----------------------------------------  
        float3 spotDiffuse = 0, spotSpecular = 0;
        SpotLight(pin, diffuseReflectance, F0, N, V, roughness, spotDiffuse, spotSpecular);
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
        finalLig += emmisive;
        
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
    
    
    finalLig.rgb = pow(finalLig.rgb, 1.0f / GammaFactor);
    return float4(finalLig, color.a);
};