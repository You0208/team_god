#include "light.hlsli"

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


float4 main(VS_OUT pin) : SV_TARGET
{
//-----------------------------------------
// �T���v�����O
//-----------------------------------------
    // �F 
    const float GAMMA = 2.2;

    float4 color = texture_maps[0].Sample(sampler_states[ANISOTROPIC], pin.texcoord);
    color.rgb = pow(color.rgb, GAMMA);
    float alpha = color.a;
    
    // �����x
    float metallic = metalness_.Sample(sampler_states[ANISOTROPIC], pin.texcoord);
    
    // ���炩��
    float smoothness = roughness_.Sample(sampler_states[ANISOTROPIC], pin.texcoord);
   
    // �@���}�b�v
    float3 normal = texture_maps[1].Sample(sampler_states[LINEAR], pin.texcoord);
    
    // �e��
    float roughness = 1.0f - smoothness;
    
    // �}�X�N
    float mask = noise_map.Sample(sampler_states[ANISOTROPIC], pin.texcoord).x;
    
//-----------------------------------------
//�@���C�e�B���O
//-----------------------------------------   
    
    float3 finalLig = 0;
    {
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
        float3 normal_ = normal * 2.0f - 1.0f;
        // ���[���h��Ԃł̖@���x�N�g���𓾂�
        float3 N = normalize(normal_.x * normalize(T) + normal_.y * normalize(B) + normal_.z * normalize(N_));
        // �J�������璸�_�ւ̕����x�N�g��
        float3 E = normalize(camera_position.xyz - pin.world_position.xyz);
        
        
        //-----------------------------------------
        // ���ڌ���PBR
        //-----------------------------------------   
        float3 directDiffuse = 0, directSpecular = 0;
        DirectBDRF(diffuseReflectance, F0, N, E, directional_light_direction.xyz,
         directional_light_color.rgb, roughness,
         directDiffuse, directSpecular);
        
        // �ŏI���ɑ������킹��
        finalLig += (directDiffuse + directSpecular);
        
        //-----------------------------------------
        // �|�C���g���C�g��PBR
        //-----------------------------------------  
        float3 pointDiffuse = 0, pointSpecular = 0;
        PointLight(pin, diffuseReflectance, F0, N, E, roughness, pointDiffuse, pointSpecular);
        // �ŏI���ɑ������킹�� 
        finalLig += (pointDiffuse + pointSpecular);
        
        //-----------------------------------------
        // �X�|�b�g���C�g��PBR
        //-----------------------------------------  
        float3 spotDiffuse = 0, spotSpecular = 0;
        SpotLight(pin, diffuseReflectance, F0, N, E, roughness, spotDiffuse, spotSpecular);
        // �ŏI���ɑ������킹��         
        finalLig += (spotDiffuse + spotSpecular);
        
        //-----------------------------------------
        //�@�V���h�E
        //-----------------------------------------     
        float3 shadow_factor = 1.0f;
        Shadow(pin, normal, T, B, shadow_map, comparison_sampler_state, shadow_factor);
        // �ŏI���ɑ������킹��    
        finalLig *= shadow_factor;        
    }
 
//-----------------------------------------
//�@�f�B�]���u
//----------------------------------------- 
    {
        float dissolve = step(mask, threshold);
        color.a *= dissolve;
        // �A���t�@��0�ȉ��Ȃ炻�������`�悵�Ȃ��悤�ɂ���
        clip(color.a - 0.01f);
    }
    

    return float4(finalLig, color.a);
};