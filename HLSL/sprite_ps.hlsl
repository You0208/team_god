#include "sprite.hlsli"
#include "ConstantBuffer.hlsli"

#if 1
Texture2D color_map : register(t0);
SamplerState point_sampler_state : register(s0);
SamplerState linear_sampler_state : register(s1);
SamplerState anisotropic_sampler_state : register(s2);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = color_map.Sample(anisotropic_sampler_state, pin.texcoord);
    float alpha = color.a;
#if 1
    // Inverse gamma process
    const float GAMMA = 2.2;
    color.rgb = pow(color.rgb,1.0f/ GAMMA);
#endif
    return float4(color.rgb, alpha) * pin.color;
}
#else

Texture2D color_map : register(t0);
Texture2D mask_texture : register(t10);

SamplerState point_sampler_state : register(s0);
SamplerState linear_sampler_state : register(s1);
SamplerState anisotropic_sampler_state : register(s2);

float4 main(VS_OUT pin) : SV_TARGET
{
    
    float4 color = color_map.Sample(anisotropic_sampler_state, pin.texcoord);
    float mask_value = mask_texture.Sample(point_sampler_state, pin.texcoord);
    //color.a *= mask_value;
#if 0
    // Inverse gamma process
    const float GAMMA = 2.2;
    color.rgb = pow(color.rgb, GAMMA);
#endif
    // stepä÷êîÇópÇ¢ÇƒìßâﬂílÇ0/1Ç…Ç∑ÇÈ
    float alpha = step(parameters.x, mask_value);
    color.a *= alpha;
    return float4(color.rgb, alpha) * pin.color;
}
#endif