#include "light.hlsli"

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
#define LINEAR_BORDER_BLACK 3
#define LINEAR_BORDER_WHITE 4

SamplerState sampler_states[3] : register(s0);
Texture2D texture_maps[8] : register(t0);

Texture2D metalSmoothTex : register(t3);
Texture2D metalTex : register(t4);
Texture2D SmoothTex : register(t5);


//Texture2D d_color : register(t20);
//Texture2D d_normal : register(t21);
//Texture2D d_MS : register(t22);

Texture2D noise_map : register(t9); // �搶�m�C�Y��3D

// SHADOW
SamplerComparisonState comparison_sampler_state : register(s5);
Texture2D shadow_map : register(t8);
Texture2D Edge : register(t11);


float4 main(VS_OUT pin) : SV_TARGET
{
//-----------------------------------------
// �T���v�����O
//-----------------------------------------
    static const float GAMMA = 2.2;

    // �F
    float4 color = texture_maps[0].Sample(sampler_states[ANISOTROPIC], pin.texcoord);
    color.rgb = pow(color.rgb, GAMMA);
    float alpha = color.a;

    // �����x
    float metallic = metalTex.Sample(sampler_states[ANISOTROPIC], pin.texcoord).r;
    
    // ���炩��
    float smoothness = SmoothTex.Sample(sampler_states[ANISOTROPIC], pin.texcoord).r;
    // ���t�l�X�̃e�N�X�`�����Ȃ����̉��}���u�̂��ߗv����
    if (smoothness <= 0.005f)
        smoothness = 0.5f;
    
    // �@���}�b�v
    float3 normal = texture_maps[1].Sample(sampler_states[LINEAR], pin.texcoord);
    
    // �e��
    float roughness = smoothness;
    
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
    
//-----------------------------------------
//�@�֊s��
//----------------------------------------- 
#if 1
    // ���K���X�N���[�����W�n����UV���W�n�ɕϊ�����
        float2 uv = pin.posInProj.xy * float2(0.5f, -0.5f) + 0.5f;
        
        // �ߖT8�e�N�Z���ւ�UV�I�t�Z�b�g
        float2 uvOffset[8] =
        {
        float2(0.0f, 1.0f / 720.0f), //��
        float2(0.0f, -1.0f / 720.0f), //��
        float2(1.0f / 1280.0f, 0.0f), //�E
        float2(-1.0f / 1280.0f, 0.0f), //��
        float2(1.0f / 1280.0f, 1.0f / 720.0f), //�E��
        float2(-1.0f / 1280.0f, 1.0f / 720.0f), //����
        float2(1.0f / 1280.0f, -1.0f / 720.0f), //�E��
        float2(-1.0f / 1280.0f, -1.0f / 720.0f) //����
        };

    // ���̃s�N�Z���̐[�x�l���擾
        float depth = Edge.Sample(sampler_states[ANISOTROPIC], uv).x;

    // �ߖT8�e�N�Z���̐[�x�l�̕��ϒl���v�Z����
        float depth2 = 0.0f;
        for (int i = 0; i < 8; i++)
        {
            depth2 += Edge.Sample(sampler_states[ANISOTROPIC], uv + uvOffset[i]).x;
        }
        depth2 /= 8.0f;

    // ���g�̐[�x�l�ƋߖT8�e�N�Z���̐[�x�l�̍��𒲂ׂ�
        if (abs(depth - depth2) > 0.00005f)
        {
        // �[�x�l�����\�Ⴄ�ꍇ�̓s�N�Z���J���[�����ɂ���
        // ->���ꂪ�G�b�W�J���[�ƂȂ�
            return float4(1.0f, 1.0f, 1.0f, 0.5f);
        }
    
#endif
    
    //finalLig = pow(finalLig, 1.0f / GAMMA);
    return float4(finalLig, alpha);
};