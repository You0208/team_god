#include "defefferd_light.hlsli"

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
#define LINEAR_BORDER_BLACK 3
#define LINEAR_BORDER_WHITE 4
SamplerState sampler_states[5] : register(s0);
Texture2D texture_maps[6] : register(t0);
float4 main(VS_OUT pin) : SV_TARGET
{
	// �K���}�W��
    static const float GammaFactor = 2.2f;
    
    // color
    float4 color = texture_maps[0].Sample(sampler_states[LINEAR_BORDER_BLACK], pin.texcoord);
    color.rgb = pow(color.rgb, GammaFactor);
    
    // Normal
    float3 normal = texture_maps[1].Sample(sampler_states[LINEAR_BORDER_BLACK], pin.texcoord).rgb;
    // position
    float3 position = texture_maps[2].Sample(sampler_states[LINEAR_BORDER_BLACK], pin.texcoord).rgb;
    float alpha = color.a;
    // �����x
    float metallic = texture_maps[3].Sample(sampler_states[LINEAR_BORDER_BLACK], pin.texcoord).r;
    // ���炩��
    float smoothness = texture_maps[3].Sample(sampler_states[LINEAR_BORDER_BLACK], pin.texcoord).a;
    // �G�~�b�V�u
    float4 emmisive = texture_maps[4].Sample(sampler_states[LINEAR_BORDER_BLACK], pin.texcoord);    
    // �I�N���[�W����
    float4 occlusion = texture_maps[5].Sample(sampler_states[LINEAR_BORDER_BLACK], pin.texcoord);
    
    // �e��
    float roughness = 1.0f - smoothness;
    
    //-----------------------------------------
    //�@���C�e�B���O
    //-----------------------------------------   
    float3 finalLig = 0;
    {
        // ���ˌ��̂����g�U���˂ɂȂ銄��
        float3 diffuseReflectance = lerp(color.rgb, 0.02f, metallic);
        // �������ˎ��̃t���l�����˗�
        float3 F0 = lerp(Dielectric, color.rgb, metallic);
        // ���[���h��Ԃł̖@���x�N�g���𓾂�
        float3 N = normal.xyz;
	    // �����x�N�g��
        float3 V = normalize(position.xyz - camera_position.xyz);
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
        PointLight(position, diffuseReflectance, F0, N, V, roughness, pointDiffuse, pointSpecular);
        // �ŏI���ɑ������킹�� 
        finalLig += (pointDiffuse + pointSpecular);
        
        //-----------------------------------------
        // �X�|�b�g���C�g��PBR
        //-----------------------------------------  
        float3 spotDiffuse = 0, spotSpecular = 0;
        SpotLight(position, diffuseReflectance, F0, N, V, roughness, spotDiffuse, spotSpecular);
        // �ŏI���ɑ������킹��         
        finalLig += (spotDiffuse + spotSpecular);
    }
     // �G�~�b�V�u
    finalLig += emmisive;
        
    // �����̏������K��
    // �I�N���[�W����
    if (occlusion.x > 0)
        finalLig = lerp(finalLig, finalLig * occlusion.xyz, 1.0f);
    
    finalLig.rgb = pow(finalLig.rgb, 1.0f / GammaFactor);
    
#if 1
	// Tone mapping : HDR -> SDR
    const float exposure = 1.2;
    color.rgb = 1 - exp(-color.rgb * exposure);
#endif

    return float4(finalLig, alpha);
}
