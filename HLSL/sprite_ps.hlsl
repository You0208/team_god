#include "sprite.hlsli"

#if 0
Texture2D color_map : register(t0);
SamplerState point_sampler_state : register(s0);
SamplerState linear_sampler_state : register(s1);
SamplerState anisotropic_sampler_state : register(s2);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = color_map.Sample(anisotropic_sampler_state, pin.texcoord);
    float alpha = color.a;
#if 0
    // Inverse gamma process
    const float GAMMA = 2.2;
    color.rgb = pow(color.rgb, GAMMA);
#endif
    return float4(color.rgb, alpha) * pin.color;
}
#else

#include "ConstantBuffer.hlsli"

Texture2D color_map : register(t0);
SamplerState color_sampler_state : register(s0);
Texture2D mask_texture : register(t10);
float4 main(VS_OUT pin) : SV_TARGET
{
    
    float4 color = color_map.Sample(color_sampler_state, pin.texcoord) * pin.color;
    float mask_value = mask_texture.Sample(color_sampler_state, pin.texcoord);
    //color.a *= mask_value;
#if 1
    // Inverse gamma process
    const float GAMMA = 2.2;
    color.rgb = pow(color.rgb, GAMMA);
#endif
    // stepä÷êîÇópÇ¢ÇƒìßâﬂílÇ0/1Ç…Ç∑ÇÈ
    float alpha = step(parameters.x, mask_value);
    color.a *= alpha;
    return color;
}
#endif