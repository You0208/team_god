// FOG
#include "fullscreen_quad.hlsli"
// NOISE
#include "noise.hlsli"

#include "ConstantBuffer.hlsli"

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
#define LINEAR_BORDER_BLACK 4
#define LINEAR_BORDER_WHITE 5
SamplerState sampler_states[5] : register(s0);
Texture2D texture_maps[4] : register(t0);



float fog_density_distribution(float3 d /*ray path*/, float3 o /*camera position*/, float b /*global density*/, float c /*height falloff*/)
{
	// https://developer.download.nvidia.com/presentations/2007/D3DTutorial_Crytek.pdf page.17-18
    float f = 0; // fog density distribution
    f = b * exp(-c * o.y) * length(d) * (1.0 - exp(-c * d.y)) / (c * d.y);
    return max(0.0, 1.0 - exp(-f));
}

float4 main(VS_OUT pin) : SV_TARGET
{
    float depth = texture_maps[0].Sample(sampler_states[LINEAR_BORDER_BLACK], pin.texcoord).x;
    
    float4 position;
	// texture space to ndc
    position.x = pin.texcoord.x * +2 - 1;
    position.y = pin.texcoord.y * -2 + 1;
    position.z = depth;
    position.w = 1;

	// ndc to world space
    position = mul(position, inv_view_projection);
    position = position / position.w;

    float3 v = position.xyz;
    float3 o = camera_position.xyz;

    float3 d = v - o; // ray path
    const float b = fog_density;
    const float c = fog_height_falloff;
    float f = fog_density_distribution(d /*ray path*/, o /*camera position*/, b /*global density*/, c /*height falloff*/);
    
 
#if 1
    const float distance = length(d);

	// start_distance : Distance from the camera that the fog will start.
    f *= exp(-0.1 * max(0.0, start_distance - distance));

	// fog_cutoff_distance : Scene elements past this distance will not have fog applied. This is useful for excluding skyboxes that already have fog baked into them.
    f *= exp(-0.1 * max(0.0, distance - fog_cutoff_distance));
#endif

    float3 color = fog_color.rgb;
    float fog_intensity = fog_color.w;

#if 1
	// Fractal Brownian Motion - lightweight version
    float t = time * time_scale;
    float3 p = position.xyz * seed_scale;
    color *= fbm(p + normalize(float3(1.0, 0.0, 0.5) /*wind velocity*/) * t);
#endif

    color = lerp(0, color, f);
    return float4(color * fog_intensity, 1);
}

