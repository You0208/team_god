#include "light.hlsli"

VS_OUT main(VS_IN vin)
{
    vin.normal.w = 0;
    float sigma = vin.tangent.w;
    vin.tangent.w = 0;

    
    float4 blended_position = { 0, 0, 0, 1 };
    float4 blended_normal = { 0, 0, 0, 0 };
    float4 blended_tangent = { 0, 0, 0, 0 };

    for (int bone_index = 0; bone_index < 4; ++bone_index)
    {
        blended_position += vin.bone_weights[bone_index] * mul(vin.position, bone_transforms[vin.bone_indices[bone_index]]);
        blended_normal += vin.bone_weights[bone_index] * mul(vin.normal, bone_transforms[vin.bone_indices[bone_index]]);
        blended_tangent += vin.bone_weights[bone_index] * mul(vin.tangent, bone_transforms[vin.bone_indices[bone_index]]);
    }
    vin.position = float4(blended_position.xyz, 1.0f);
    vin.normal = float4(blended_normal.xyz, 0.0f);
    vin.tangent = float4(blended_tangent.xyz, 0.0f);
    
    VS_OUT vout;
    vout.position = mul(vin.position, mul(world, view_projection));

    vout.world_position = mul(vin.position, world);
    vin.normal.w = 0;
    vout.world_normal = normalize(mul(vin.normal, world));
    vout.world_tangent = normalize(mul(vin.tangent, world));
    vout.world_tangent.w = sigma;

    vout.texcoord = vin.texcoord + parameters.z;
#if 1
    vout.color = material_color;
#else
    // 頂点影響力を視覚的に確認する
    vout.color = 0;
    const float4 bone_colors[4] =
    {
        { 1, 0, 0, 1 },
        { 0, 1, 0, 1 },
        { 0, 0, 1, 1 },
        { 1, 1, 1, 1 },
    };
    for (int bone_index = 0;bone_index < 4; ++bone_index)
    {
        vout.color += bone_colors[vin.bone_indices[bone_index] % 4] * vin.bone_weights[bone_index];
    }

#endif
    vout.binormal = normalize(cross(vout.world_normal.xyz, vout.world_tangent.xyz));
    
    
    //// Calculate view direction in world space
    //float3 viewDirection = normalize(camera_position.xyz - mul(world, float4(vin.position.xyz, 1.0f)).xyz);

    
    //// Construct TBN matrix
    //float3x3 TBN = float3x3(vout.world_tangent.xyz, vout.binormal.xyz, vout.world_normal.xyz);
    //vout.view_tan= mul(viewDirection, TBN);
    
    
    return vout;
}