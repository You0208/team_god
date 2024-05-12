#include "sprite.hlsli"

#include "ConstantBuffer.hlsli"

Texture2D color_map : register(t0);
SamplerState color_sampler_state : register(s0);
Texture2D mask_texture : register(t10);
float4 main(VS_OUT pin) : SV_TARGET
{
    
    float4 color = color_map.Sample(color_sampler_state, pin.texcoord) * pin.color;
    float mask_value = mask_texture.Sample(color_sampler_state, pin.texcoord);
    //color.a *= mask_value;
#if 0
    // Inverse gamma process
    const float GAMMA = 2.2;
    color.rgb = pow(color.rgb, 1 / GAMMA);
#endif
    // step�֐���p���ē��ߒl��0/1�ɂ���
    float alpha = step(parameters.x, mask_value);
    color.a *= alpha;
    return color;
}