
#include "light.hlsli"
Texture2D Edge : register(t11);

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
#define LINEAR_BORDER_BLACK 3
#define LINEAR_BORDER_WHITE 4

SamplerState sampler_states[3] : register(s0);
float4 main(VS_OUT pin) : SV_TARGET
{
     
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
        return float4(parameters.x, parameters.y, parameters.w, 0.6f);
    }
    
#endif
    
    
    //return float4(1.0f, 1.0f, 0.9f, 0.2f);// �[���_�z���C�g
    return float4(1.0f, 0.5f, 0.0f, 0.1f);
}