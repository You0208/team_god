#include "skinned_mesh.hlsli"

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
SamplerState sampler_states[3] : register(s0);
Texture2D texture_maps[4] : register(t0);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = texture_maps[0].Sample(sampler_states[ANISOTROPIC], pin.texcoord);
    float alpha = color.a;

#if 1
	// Inverse gamma process
    const float GAMMA = 2.2;
    color.rgb = pow(color.rgb, GAMMA);
#endif

    float3 N = normalize(pin.world_normal.xyz);
#if 1
    float3 T = normalize(pin.world_tangent.xyz);
    float sigma = pin.world_tangent.w;
    T = normalize(T - N * dot(N, T));
    float3 B = normalize(cross(N, T) * sigma);

    float4 normal = texture_maps[1].Sample(sampler_states[LINEAR], pin.texcoord);
    normal = (normal * 2.0) - 1.0;
    N = normalize((normal.x * T) + (normal.y * B) + (normal.z * N));
#endif

    float3 L = normalize(-directional_light_direction.xyz);
    float3 V = normalize(camera_position.xyz - pin.world_position.xyz);
    float3 H = normalize(L + V);

    float NoL = max(0, dot(N, L));
#if 0
	float NoH = max(0, dot(normalize(pin.world_normal.xyz), H));
#else
    float NoH = max(0, dot(N, H));
#endif
    float NoV = max(0, dot(N, V));

	
    float irradiance = smoothstep(0.0, 0.01, NoL);
	
    float3 ambient_color = color.rgb;
    float3 ambient = lerp(0.4 * ambient_color, 0, irradiance);
	
    float3 diffuse_color = color.rgb;
    float3 diffuse = irradiance * diffuse_color;
	
    float specular_intensity = pow(NoH, 512);
    float specular_intensity_smooth = smoothstep(0.005, 0.01, specular_intensity);
    float3 specular_color = color.rgb;
    float3 specular = specular_intensity_smooth * specular_color;
	
    const float rim_threshold = 2.0;
    const float rim_amount = 0.716;
    float rim_dot = 1 - NoV;
    float rim_intensity = rim_dot * pow(NoL, rim_threshold);
    rim_intensity = smoothstep(rim_amount - 0.1, rim_amount + 0.1, rim_intensity);
    float3 rom_color = 1;
    float3 rim = rim_intensity * rom_color;
	
    return float4(ambient + diffuse + specular + rim, alpha) * pin.color;
}

