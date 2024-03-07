#include "bidirectional_reflectance_distribution_function.hlsli"

struct VS_IN
{
    float4 position : POSITION;
    float4 normal : NORMAL;
    float4 tangent : TANGENT;
    float2 texcoord : TEXCOORD;
    uint4 joints : JOINTS;
    float4 weights : WEIGHTS;
};
struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 w_position : POSITION;
    float4 w_normal : NORMAL;
    float4 w_tangent : TANGENT;
    float2 texcoord : TEXCOORD;
    
    float3 binormal : BINORMAL;
};

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
// 半球ライト
cbuffer HEMISPHERE_LIGHT_CONSTANT_BUFFER : register(b7)
{
    float4 sky_color;
    float4 ground_color;
    float4 hemisphere_weight;
};

cbuffer SCENE_CONSTANT_BUFFER : register(b1)
{
    row_major float4x4 view_projection;
    //float4 light_direction;
    float4 camera_position;
    row_major float4x4 inverse_projection;
    float time;
    float3 pad;
    row_major float4x4 inv_view_projection;
    // SHADOW
    row_major float4x4 light_view_projection;
    float shadow_depth_bias;
    float pads2[3];
};

cbuffer PRIMITIVE_CONSTANT_BUFFER : register(b0)
{
    row_major float4x4 world;
    int material;
    bool has_tangent;
    int skin;
    int pad3;
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

struct texture_info
{
    int index;
    int texcoord;
};
struct normal_texture_info
{
    int index;
    int texcoord;
    float scale;
};
struct occlusion_texture_info
{
    int index;
    int texcoord;
    float strength;
};
struct pbr_metallic_roughness
{
    float4 basecolor_factor;
    texture_info basecolor_texture;
    float metallic_factor;
    float roughness_factor;
    texture_info metallic_roughness_texture;
};
struct material_constants
{
    float3 emissive_factor;
    int alpha_mode; // "OPAQUE" : 0, "MASK" : 1, "BLEND" : 2
    float alpha_cutoff;
    bool double_sided;
    pbr_metallic_roughness pbr_metallic_roughness;
    normal_texture_info normal_texture;
    occlusion_texture_info occlusion_texture;
    texture_info emissive_texture;
};
StructuredBuffer<material_constants> materials : register(t0);

static const uint PRIMITIVE_MAX_JOINTS = 512;
cbuffer PRIMITIVE_JOINT_CONSTANTS : register(b2)
{
    row_major float4x4 joint_matrices[PRIMITIVE_MAX_JOINTS];
};


// フォグ
float4 CalcFog(in float4 color, float4 fog_color, float2 fog_range, float eye_length)
{
    float fogAlpha = saturate((eye_length - fog_range.x) / (fog_range.y - fog_range.x));
    return lerp(color, fog_color, fogAlpha);
}


//-----------------------シャドウ------------------------------------------------

void Shadow(VS_OUT pin, float3 normal, float3 T, float3 B,
Texture2D shadow_map, SamplerComparisonState comparison_sampler_state, out float3 shadow_factor)
{
    float3 L = normalize(-directional_light_direction.xyz);
    float3 SN = normalize(pin.w_normal.xyz);
    // 接ベクトル        
    SN = normalize((normal.x * T) + (normal.y * B) + (normal.z * SN));
    const float shadow_depth_bias = max(0.01 * (1.0 - dot(SN, L)), 0.001);
        
    float4 light_view_position = mul(pin.w_position, light_view_projection);
    light_view_position = light_view_position / light_view_position.w;
    float2 light_view_texcoord = 0;
    light_view_texcoord.x = light_view_position.x * +0.5 + 0.5;
    light_view_texcoord.y = light_view_position.y * -0.5 + 0.5;
    float depth = saturate(light_view_position.z - shadow_depth_bias);
        
    shadow_factor = shadow_map.SampleCmpLevelZero(comparison_sampler_state, light_view_texcoord, depth).xxx;
}

//-----------------------ライティングその他------------------------------------------------

//--------------------------------------------
// リムライト
//--------------------------------------------
// N:法線(正規化済み)
// E:視点方向ベクトル(正規化済み)
// L:入射ベクトル(正規化済み)
// C :ライト色
// RimPower : リムライトの強さ(初期値はテキトーなので自分で設定するが吉)
float3 CalcRimLight(float3 normal, float3 E, float3 L, float3 lightColor, float RimPower = 3.0f)
{
    float rim = 1.0f - saturate(dot(normal, -E));
    return lightColor * pow(rim, RimPower) * saturate(dot(L, -E));
}

//--------------------------------------------
// 半球ライティング
//--------------------------------------------
// normal:法線(正規化済み)
// up:上方向（片方）
// sky_color:空(上)色
// ground_color:地面(下)色
// hemisphere_weight:重み
float3 CalcHemiSphereLight(float3 normal, float3 up, float3 sky_color, float3 ground_color, float4 hemisphere_weight)
{
    float factor = dot(normal, up) * 0.5f + 0.5f;
    return lerp(ground_color, sky_color, factor) * hemisphere_weight.x;
}

// リムライト
void LimHemiLight(VS_OUT pin,
float3 normal,
float3 N_,
float3 T,
float3 E,
out float3 rimColor,
out float3 hemiLight
)
{
    // ライト方向ベクトル
    float3 Light = normalize(directional_light_direction.xyz);
     // 各ベクトルを正規化し、それらを行列の各列に割り当て
    float3x3 mat = { T, pin.binormal, N_ };
     // ノーマルテクスチャ法線をワールドへ変換
    normal = normalize(mul(normal * 2.0f - 1.0f, mat));
     
    rimColor = CalcRimLight(normal, -E, Light, directional_light_color.rgb);
     // 半球ライト
    hemiLight = CalcHemiSphereLight(normal, float3(0, 1, 0), sky_color.rgb, ground_color.rgb, hemisphere_weight);
    
}


/// <summary>
/// Lambert拡散反射光を計算する
/// </summary>
float3 CalcLambertDiffuse(float3 lightDirection, float3 lightColor, float3 normal)
{
    // ピクセルの法線とライトの方向の内積を計算する
    float t = dot(normal, lightDirection) * -1.0f;

    // 内積の値を0以上の値にする
    t = max(0.0f, t);

    // 拡散反射光を計算する
    return lightColor * t;
}

//-----------------------PBRライティング------------------------------------------------

// ポイントライト
void PointLight(VS_OUT pin,
float3 N,
float3 V,
float3 f0,
float3 f90,
float alpha_roughness,
float3 c_diff,
out float3 outDiffuse,
out float3 outSpecular)
{
    for (int i = 0; i < 8; ++i)
    {
        // サーフェイスに入射するライトの光の向きを計算する
        float3 lightVector = pin.w_position.xyz - point_light[i].position.xyz;
        float lightLength = length(lightVector);
        if (lightLength >= point_light[i].range)
            continue;
        float attenuate = saturate(1.0f - lightLength / point_light[i].range);
        lightVector = lightVector / lightLength;
        
        // ポイントライトとの距離を計算する
        float3 distance = length(pin.w_position.xyz - point_light[i].position.xyz);
        // 影響率は距離に比例して小さくなっていく
        float affect = 1.0f - 1.0f / point_light[i].range.x * distance;
        // 影響力がマイナスにならないように補正をかける
        if (affect < 0.0f)
        {
            affect = 0.0f;
        }
        //影響を指数関数的にする。今回のサンプルでは3乗している
        affect = pow(affect, 3.0f);
        
        float3 diffuse = 0;
        float3 specular = 0;
    
        // ライトごとのシェーディング処理のループ
        float3 L = lightVector;
        float3 Li = point_light[i].color; // 光の輝き
    
        // 法線ベクトルと光源へのベクトルとの内積
        const float NoL = max(0.0, dot(N, L));
        // 法線ベクトルと視線へのベクトルとの内積
        const float NoV = max(0.0, dot(N, V));
    
        // 内積が0じゃないなら（垂直でないなら）
        if (NoL > 0.0 || NoV > 0.0)
        {
            const float3 R = reflect(-L, N); // 反射ベクトル
            const float3 H = normalize(V + L); // ハーフベクトル（光源へのベクトルと視点へのベクトルの中間ベクトル）
        
            const float NoH = max(0.0, dot(N, H)); // 法線ベクトルとハーフベクトルの内積
            const float HoV = max(0.0, dot(H, V)); // VoH: 視線ベクトルとハーフベクトルの内積
        
            diffuse += Li * NoL * brdf_lambertian(f0, f90, c_diff, HoV);
            specular += Li * NoL * brdf_specular_ggx(f0, f90, alpha_roughness, HoV, NoL, NoV, NoH);
        }
        
        outDiffuse = diffuse * attenuate;
        outSpecular = specular * attenuate;
    }
}

// スポットライト
void SpotLight(VS_OUT pin,
float3 N,
float3 V,
float3 f0,
float3 f90,
float alpha_roughness,
float3 c_diff,
out float3 outDiffuse,
out float3 outSpecular
)
{
    for (int j = 0; j < 1; ++j)
    {
        float3 diffSpotLight = 0;
        float3 specSpotLight = 0;
        // サーフェイスに入射するスポットライトの光の向きを計算する
        float3 L =spot_light[j].direction.xyz;
        // スポットライトのカラー
        float3 Li = spot_light[j].color.xyz;
       // outDiffuse = L;
           // 法線ベクトルと光源へのベクトルとの内積
        const float NoL = max(0.0, dot(N, L));
        // 法線ベクトルと視線へのベクトルとの内積
        const float NoV = max(0.0, dot(N, V));
    
        // 内積が0じゃないなら（垂直でないなら）
        if (NoL > 0.0 || NoV > 0.0)
        {
            const float3 R = reflect(-L, N); // 反射ベクトル
            const float3 H = normalize(V + L); // ハーフベクトル（光源へのベクトルと視点へのベクトルの中間ベクトル）
        
            const float NoH = max(0.0, dot(N, H)); // 法線ベクトルとハーフベクトルの内積
            const float HoV = max(0.0, dot(H, V)); // VoH: 視線ベクトルとハーフベクトルの内積
        
            diffSpotLight += Li * NoL * brdf_lambertian(f0, f90, c_diff, HoV);
            specSpotLight += Li * NoL * brdf_specular_ggx(f0, f90, alpha_roughness, HoV, NoL, NoV, NoH);
        }
        
        // 正規化して大きさ1のベクトルにする
        L = normalize(L);
        
        // 距離による影響率を計算する
        // スポットライトとの距離を計算する
        float3 distance = length(pin.w_position.xyz - spot_light[j].position.xyz);

        // 影響率は距離に比例して小さくなっていく
        float affect = 1.0f - 1.0f / spot_light[j].range * distance;
        
        // 影響力がマイナスにならないように補正をかける
        if (affect < 0.0f)
        {
            affect = 0.0f;
        }

        // 影響の仕方を指数関数的にする
        affect = pow(affect, 6.0f);
        
        // 影響率を乗算して反射光を弱める
        diffSpotLight *= affect;
        specSpotLight *= affect;
        
        // 入射光と射出方向の角度を求める
        // dot()を利用して内積を求める
        float angle = dot(L, spot_light[j].direction.xyz);
        
        // dot()で求めた値をacos()に渡して角度を求める
        angle = abs(acos(angle));
        
        // 角度による影響率を求める
        // 角度に比例して小さくなっていく影響率を計算する
        affect = 1.0f - 1.0f / spot_light[j].angle * angle;

        // 影響率がマイナスにならないように補正をかける
        if (affect < 0.0f)
        {
            affect = 0.0f;
        }
        
        // 影響の仕方を指数関数的にする。今回のサンプルでは0.5乗している
        affect = pow(affect, 0.5f);

        // 角度による影響率を反射光に乗算して、影響を弱める
        diffSpotLight *= affect;
        specSpotLight *= affect;
        
        outDiffuse = diffSpotLight;
        outSpecular = specSpotLight;
    }
}