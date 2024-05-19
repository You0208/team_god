#include "light.hlsli"

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
#define LINEAR_BORDER_BLACK 3
#define LINEAR_BORDER_WHITE 4

SamplerState sampler_states[5] : register(s0);
Texture2D texture_maps[8] : register(t0);

Texture2D metalSmoothTex : register(t3);
Texture2D metalTex : register(t4);
Texture2D SmoothTex : register(t5);
Texture2D Opacity : register(t6);


//Texture2D d_color : register(t20);
//Texture2D d_normal : register(t21);
//Texture2D d_MS : register(t22);

Texture2D noise_map : register(t9); // �搶�m�C�Y��3D

// SHADOW
SamplerComparisonState comparison_sampler_state : register(s5);
Texture2D shadow_map : register(t8);

// PROJECTION_MAPPING
Texture2D projection_mapping_texture : register(t15);


float4 main(VS_OUT pin) : SV_TARGET
{
//-----------------------------------------
// �T���v�����O
//-----------------------------------------
    // �C��
    static const float GAMMA = 2.2;
    // �F
    float4 color = texture_maps[0].Sample(sampler_states[ANISOTROPIC], pin.texcoord);
    color.rgb = pow(color.rgb, GAMMA);
    float alpha = color.a;
    // �����x
    float metallic = metalTex.Sample(sampler_states[ANISOTROPIC], pin.texcoord).r;
    //float metallic = parameters.x;
    
    // ���炩��
    float smoothness = SmoothTex.Sample(sampler_states[ANISOTROPIC], pin.texcoord).r;
    // ���t�l�X�̃e�N�X�`�����Ȃ����̉��}���u�̂��ߗv����
    if (smoothness <= 0.005f)
        smoothness = 0.5f;
    // �e��
    //float roughness = 1.0f - smoothness;
    float roughness = smoothness;
    //float roughness = parameters.y;
    // �@���}�b�v
    float3 normal = texture_maps[1].Sample(sampler_states[LINEAR], pin.texcoord);
    
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
        // �����x�N�g��
        float3 V = normalize(pin.world_position.xyz - camera_position.xyz);
        
        
        //-----------------------------------------
        // ���ڌ���PBR
        //-----------------------------------------   
        float3 directDiffuse = 0, directSpecular = 0;
        float3 L = normalize(directional_light_direction.xyz);
        DirectBDRF(diffuseReflectance, F0, N, V, L,
			directional_light_color.rgb, roughness, directDiffuse, directSpecular);
        // �ŏI���ɑ������킹��
        finalLig += (directDiffuse + directSpecular);
        
        //return float4(roughness,0,0, 1);
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
        //return float4(shadow_factor, 1);
        //-----------------------------------------
        //�@�������C�g
        //----------------------------------------- 
        float3 rimColor = 0, hemiLight = 0;
        LimHemiLight(pin, normal, N_, T, E, rimColor, hemiLight);
        // �ŏI���ɑ������킹��     
        finalLig += (rimColor + hemiLight);
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
    
	// PROJECTION_MAPPING
    const float projection_mapping_color_intensity = 10;
    float3 projection_mapping_color = 0;
    float4 projection_texture_position = mul(pin.world_position, projection_mapping_transform);
    projection_texture_position /= projection_texture_position.w;
    projection_texture_position.x = projection_texture_position.x * 0.5 + 0.5;
    projection_texture_position.y = -projection_texture_position.y * 0.5 + 0.5;
    if (saturate(projection_texture_position.z) == projection_texture_position.z)
    {
        float4 projection_texture_color = projection_mapping_texture.Sample(sampler_states[LINEAR_BORDER_BLACK], projection_texture_position.xy);
        projection_mapping_color = projection_texture_color.rgb * projection_texture_color.a * projection_mapping_color_intensity;
    }

// PostEffect�̂Ƃ��؂�    
#if 0
    finalLig = pow(finalLig, 1.0f / GAMMA);
#endif
    return float4(finalLig + projection_mapping_color/*PROJECTION_MAPPING*/, color.a);
};