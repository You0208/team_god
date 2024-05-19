
// ライト用
struct point_lights
{
    float4 position;
    float4 color;
    float range;
    float3 dummy;
};
// スポットライト
struct spot_lights
{
    float4 position;
    float4 direction;
    float4 color;
    float range;
    float3 pad;
    float angle;
    float3 pad2;
};

// 円周率
static const float PI = 3.141592654f;
static const int MAX_BONES = 256;


cbuffer OBJECT_CONSTANT_BUFFER : register(b0)
{
    row_major float4x4 world;
    float4 material_color;
    row_major float4x4 bone_transforms[MAX_BONES];
    float threshold;
    float3 pads;
};

cbuffer SCENE_CONSTANT_BUFFER : register(b1)
{
    row_major float4x4 view_projection;
    
    float4 camera_position;
    row_major float4x4 inverse_projection;
    float time;
    float3 pad;
    row_major float4x4 inv_view_projection;
    
    // SHADOW
    row_major float4x4 light_view_projection;
    float shadow_depth_bias;
    float3 pads2;
    
    // particle
    row_major float4x4 view_matrix;
    row_major float4x4 projection_matrix;
    
	// PROJECTION_MAPPING
    row_major float4x4 projection_mapping_transform;
};

cbuffer FOG_CONSTANT_BUFFER : register(b2)
{
    float4 fog_color;
    float fog_density;
    float fog_height_falloff;
    float start_distance;
    float fog_cutoff_distance;
    float time_scale;
    float seed_scale;
};

cbuffer OPTION_CONSTANT_BUFFER : register(b3)
{
    float4 parameters; // x : ディゾルブ適応量、z：UVスクロール、yw : 空き
    float4 rgb_adjustment;
    float4 hsv_adjustment;
};


// PBR調整のための定数バッファーを用意する
cbuffer DirectionLightCb : register(b4)
{
    float4 materialColor;
    float4 adjustMetalness;
    float4 adjustSmoothness;
};

// FOG
cbuffer FOG_CONSTANT_BUFFER : register(b5)
{
    float4 dis_fog_color;
    float4 dis_fog_range;
};

// ライトデータにアクセスするための定数バッファーを用意する
cbuffer light_constant : register(b6)
{
    float4 ambient_color;
    float4 directional_light_direction;
    float4 directional_light_color;
    point_lights point_light[8];
    spot_lights spot_light[8];
};

// 半球ライト
cbuffer HEMISPHERE_LIGHT_CONSTANT_BUFFER : register(b7)
{
    float4 sky_color;
    float4 ground_color;
    float4 hemisphere_weight;
};


cbuffer BLOOM_CONSTANT_BUFFER : register(b8)
{
    float bloom_extraction_threshold;
    float bloom_intensity;
    float2 something;
};