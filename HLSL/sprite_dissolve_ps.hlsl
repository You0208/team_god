#include "sprite_dissolve.hlsli"
#include "ConstantBuffer.hlsli"

Texture2D color_map : register(t0);
SamplerState color_sampler_state : register(s0);
Texture2D mask_texture : register(t1);
float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = color_map.Sample(color_sampler_state, pin.texcoord) * pin.color;
    float mask_value = mask_texture.Sample(color_sampler_state, pin.texcoord);
    //color.a *= mask_value;
    
    // step関数を用いて透過値を0/1にする
    float alpha = step(parameters.x, mask_value);
    color.a *= alpha;
    return color;
}