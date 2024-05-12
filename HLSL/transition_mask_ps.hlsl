#include "sprite.hlsli"

Texture2D color_map : register(t0);
SamplerState color_sampler_state : register(s0);
Texture2D mask_texture : register(t10);
float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = color_map.Sample(color_sampler_state, pin.texcoord) * pin.color;
    float4 mask = mask_texture.Sample(color_sampler_state, pin.texcoord);
    //color.a *= mask_value;
#if 1
    // Inverse gamma process
    const float GAMMA = 2.2;
    color.rgb = pow(color.rgb, 1 / GAMMA);
#endif
    if (mask.a > 0)
        color.a = 0.0f;
    return color;
}