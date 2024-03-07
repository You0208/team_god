#include "gltf_model.hlsli"
#include "bidirectional_reflectance_distribution_function.hlsli"

#define BASECOLOR_TEXTURE 0
#define METALLIC_ROUGHNESS_TEXTURE 1
#define NORMAL_TEXTURE 2
#define EMISSIVE_TEXTURE 3
#define OCCLUSION_TEXTURE 4

Texture2D<float4> material_textures[5] : register(t1);


float4 main(VS_OUT pin) : SV_TARGET
{
    const float GAMMA = 2.2;
    
    material_constants m = materials[material];
    
    float4 basecolor_factor = m.pbr_metallic_roughness.basecolor_factor;
    const int basecolor_texture = m.pbr_metallic_roughness.basecolor_texture.index;
    if (basecolor_texture > -1)
    {
        float4 sampled = material_textures[BASECOLOR_TEXTURE].Sample(sampler_states[ANISOTROPIC], pin.texcoord);
        sampled.rgb = pow(sampled.rgb, GAMMA);
        basecolor_factor *= sampled;
    }
    

    
    float3 emmisive_factor = m.emissive_factor;
    const int emissive_texture = m.emissive_texture.index;
    if (emissive_texture > -1)
    {
        float4 sampled = material_textures[EMISSIVE_TEXTURE].Sample(sampler_states[ANISOTROPIC], pin.texcoord);
        sampled.rgb = pow(sampled.rgb, GAMMA);
        emmisive_factor *= sampled.rgb;
    }
    
    float roughness_factor = m.pbr_metallic_roughness.roughness_factor;
    float metallic_factor = m.pbr_metallic_roughness.metallic_factor;
    const int metallic_roughness_texture = m.pbr_metallic_roughness.metallic_roughness_texture.index;
    if (metallic_roughness_texture > -1)
    {
        float4 sampled = material_textures[METALLIC_ROUGHNESS_TEXTURE].Sample(sampler_states[LINEAR], pin.texcoord);
        roughness_factor *= sampled.g;
        metallic_factor *= sampled.b;
    }
    float occlusion_factor = 1.0;
    const int occlusion_texture = m.occlusion_texture.index;
    if (occlusion_texture > -1)
    {
        float4 sampled = material_textures[OCCLUSION_TEXTURE].Sample(sampler_states[LINEAR], pin.texcoord);
        occlusion_factor *= sampled.r;
    }
    const float occlusion_strength = m.occlusion_texture.strength;

    
    // 垂直反射時のフレネル反射率（物体の金属性と基本色に基づいて、物体の表面の光の反射率を推定）
    const float3 f0 = lerp(0.04, basecolor_factor.rgb, metallic_factor);
   
    const float3 f90 = 1.0;
    // 粗さを2乗することにより、物体の表面の微細な変動がより強調される
    const float alpha_roughness = roughness_factor * roughness_factor;
    // ?
    const float3 c_diff = lerp(basecolor_factor.rgb, 0.0, metallic_factor);

    const float3 P = pin.w_position.xyz;
    // 視線へのベクトル
    const float3 V = normalize(camera_position.xyz - pin.w_position.xyz);
    
    // 法線ベクトル
    float3 N = normalize(pin.w_normal.xyz);
    // 接線ベクトル
    float3 T = has_tangent ? normalize(pin.w_tangent.xyz) : float3(1, 0, 0);
    float sigma = has_tangent ? pin.w_tangent.w : 1.0;
    T = normalize(T - N * dot(N, T));
    // 従ベクトル
    float3 B = normalize(cross(N, T) * sigma);
    
    const int normal_texture = m.normal_texture.index;
    if (normal_texture > -1)
    {
        float4 sampled = material_textures[NORMAL_TEXTURE].Sample(sampler_states[LINEAR], pin.texcoord);
        float3 normal_factor = sampled.xyz;
        normal_factor = (normal_factor * 2.0) - 1.0;
        normal_factor = normalize(normal_factor * float3(m.normal_texture.scale, m.normal_texture.scale, 1.0));
        N = normalize((normal_factor.x * T) + (normal_factor.y * B) + (normal_factor.z * N));
    }
    
    float3 diffuse = 0;
    float3 specular = 0;
    
    // ライトごとのシェーディング処理のループ
    float3 L = normalize(-directional_light_direction.xyz);
    float3 Li = float3(1.0, 1.0, 1.0); // 光の輝き
    // 法線ベクトルと光源へのベクトルとの内積
    const float NoL = max(0.0, dot(N, L));
    // 法線ベクトルと視線へのベクトルとの内積
    const float NoV = max(0.0, dot(N, V));
    if (NoL > 0.0 || NoV > 0.0)
    {
        const float3 R = reflect(-L, N); // ?
        const float3 H = normalize(V + L); // ハーフベクトル（光源へのベクトルと視点へのベクトルの中間ベクトル）
        
        const float NoH = max(0.0, dot(N, H)); // 法線ベクトルとハーフベクトルの内積
        const float HoV = max(0.0, dot(H, V)); // VoH: 視線ベクトルとハーフベクトルの内積
        
        diffuse += Li * NoL * brdf_lambertian(f0, f90, c_diff, HoV);
        specular += Li * NoL * brdf_specular_ggx(f0, f90, alpha_roughness, HoV, NoL, NoV, NoH);
    }
    
    diffuse += ibl_radiance_lambertian(N, V, roughness_factor, c_diff, f0);
    specular += ibl_radiance_ggx(N, V, roughness_factor, f0);
    
    float3 emmisive = emmisive_factor;
    diffuse = lerp(diffuse, diffuse * occlusion_factor, occlusion_strength);
    specular = lerp(specular, specular * occlusion_factor, occlusion_strength);
    
    float3 Lo = diffuse + specular + emmisive;
    return float4(Lo, basecolor_factor.a);
    
    //float4 basecolor = m.pbr_metallic_roughness.basecolor_texture.index > -1 ?
    //material_textures[BASECOLOR_TEXTURE].Sample(sampler_states[ANISOTROPIC], pin.texcoord) :
    //m.pbr_metallic_roughness.basecolor_factor;
    
    //float3 emmisive = m.emissive_texture.index > -1 ? 
    //material_textures[EMISSIVE_TEXTURE].Sample(sampler_states[ANISOTROPIC], pin.texcoord).rgb :
    //m.emissive_factor;
    
    //float3 N = normalize(pin.w_normal.xyz);
    //float3 L = normalize(-light_direction.xyz);
    
    //float3 color = max(0, dot(N, L)) * basecolor.rgb + emmisive;
    //return float4(color, basecolor.a);
}
