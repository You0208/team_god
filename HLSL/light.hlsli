#include "ConstantBuffer.hlsli"
struct VS_IN
{
    float4 position : POSITION;
    float4 normal : NORMAL;
    float4 tangent : TANGENT;
    float2 texcoord : TEXCOORD;
    float4 bone_weights : WEIGHTS;
    uint4 bone_indices : BONES;
};

struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 world_position : POSITION;
    float4 world_normal : NORMAL;
    float4 world_tangent : TANGENT;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
    
    float3 binormal : BINORMAL;
};

// 関数宣言
void DirectBDRF(float3 diffuseReflectance, float3 F0, float3 normal, float3 eyeVector, float3 lightVector, float3 lightColor, float roughness, out float3 outDiffuse, out float3 outSpecular);


// Lambert拡散反射光を計算する
float3 CalcLambertDiffuse(float3 lightDirection, float3 lightColor, float3 normal)
{
    // ピクセルの法線とライトの方向の内積を計算する
    float t = dot(normal, lightDirection) * -1.0f;

    // 内積の値を0以上の値にする
    t = max(0.0f, t);

    // 拡散反射光を計算する
    return lightColor * t;
}

// Phong鏡面反射光を計算する
float3 CalcPhongSpecular(float3 lightDirection, float3 lightColor, float3 worldPos, float3 normal)
{
    // 反射ベクトルを求める
    float3 refVec = reflect(lightDirection, normal);

    // 光が当たったサーフェイスから視点に伸びるベクトルを求める
    float3 toEye = camera_position.xyz - worldPos;
    toEye = normalize(toEye);

    // 鏡面反射の強さを求める
    float t = dot(refVec, toEye);

    // 鏡面反射の強さを0以上の数値にする
    t = max(0.0f, t);

    // 鏡面反射の強さを絞る
    t = pow(t, 5.0f);

    // 鏡面反射光を求める
    return lightColor * t;
}

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
    float3 SN = normalize(pin.world_normal.xyz);
         // 接ベクトル
        
    SN = normalize((normal.x * T) + (normal.y * B) + (normal.z * SN));
    const float shadow_depth_bias = max(0.01 * (1.0 - dot(SN, L)), 0.001);
        
    float4 light_view_position = mul(pin.world_position, light_view_projection);
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

//-----------------------PBRライティング------------------------------------------------

// ポイントライト
void PointLight(VS_OUT pin,
float3 diffuseReflectance,
float3 F0,
float3 N,
float3 V,
float roughness,
out float3 outDiffuse,
out float3 outSpecular)
{
    for (int i = 0; i < 8; ++i)
    {
        // サーフェイスに入射するライトの光の向きを計算する
        float3 lightVector = pin.world_position.xyz - point_light[i].position.xyz;
        float lightLength = length(lightVector);
        if (lightLength >= point_light[i].range)
            continue;
        
        float attenuateLength = saturate(1.0f - lightLength / point_light[i].range);
        float attenuation = attenuateLength * attenuateLength;
        lightVector = lightVector / lightLength;
        
        float3 diffuse = 0, specular = 0;
        DirectBDRF(diffuseReflectance, F0, N, V, lightVector,
            point_light[i].color.rgb, roughness,
            diffuse, specular);
        
        outDiffuse += diffuse * attenuation;
        outSpecular += specular * attenuation;
    }
}

// スポットライト
void SpotLight(VS_OUT pin,
float3 diffuseReflectance,
float3 F0,
float3 N,
float3 E,
float roughness,
out float3 outDiffuse,
out float3 outSpecular
)
{
    for (int j = 0; j < 2; ++j)
    {
        // サーフェイスに入射するスポットライトの光の向きを計算する
        float3 ligDir = pin.world_position.xyz - spot_light[j].position.xyz;
        // スポットライトのカラー
        float3 spColor = spot_light[j].color.xyz;
        // 正規化して大きさ1のベクトルにする
        ligDir = normalize(ligDir);
        
        float3 diffSpotLight = 0;
        float3 specSpotLight = 0;

        DirectBDRF(diffuseReflectance, F0, N, E, ligDir,
            spot_light[j].color.rgb, roughness,
            diffSpotLight, specSpotLight);
        
        // 距離による影響率を計算する
        // スポットライトとの距離を計算する
        float3 distance = length(pin.world_position.xyz - spot_light[j].position.xyz);

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
        float angle = dot(ligDir, spot_light[j].direction.xyz);
        
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
        
        outDiffuse += diffSpotLight;
        outSpecular += specSpotLight;
    }
}

//------------------------- PBR部品--------------------------------------------------

// 誘電率(非金属でも最低4%(0.0f4)は鏡面反射する)
static const float Dielectric = 0.04f;
//--------------------------------------------
// フレネル項
//--------------------------------------------
// F0 : 垂直入射時の反射率
// VdotH: 視線ベクトルとハーフベクトル（光源へのベクトルと視点へのベクトルの中間ベクトル
float3 CalcFresnel(float3 F0, float VdotH)
{
    return F0 + (1.0f - F0) * pow(clamp(1.0f - VdotH, 0.0f, 1.0f), 5.0f);
}


//--------------------------------------------
// 拡散反射BRDF(正規化ランバートの拡散反射)
//--------------------------------------------
//VdotH : 視線へのベクトルとハーフベクトルとの内積
//fresnelF0 : 垂直入射時のフレネル反射色
//diffuseReflectance : 入射光のうち拡散反射になる割合
float3 DiffuseBRDF(float VdotH, float3 fresnelF0, float3 diffuseReflectance)
{
    return (1.0f - CalcFresnel(fresnelF0, VdotH)) * (diffuseReflectance / PI);
}


//--------------------------------------------
// 法線分布関数
//--------------------------------------------
// NdotH : 法線ベクトルとハーフベクトル（光源へのベクトルと視点へのベクトルの中間ベクトル）の内積
// roughness : 粗さ
float CalcNormalDistributionFunction(float NdotH, float roughness)
{
    float alpha = roughness * roughness;
    float alpha2 = alpha * alpha;
    float denom = (NdotH * NdotH) * (alpha2 - 1.0f) + 1.0f;
    return alpha2 / (PI * denom * denom);
}
//--------------------------------------------
// 幾何減衰項の算出
//--------------------------------------------
// Ndotv : 法線と他ベクトルとの内積
// k : 粗さ/2
float CalcGeometryFunction(float Ndotv, float k)
{
    return Ndotv / (Ndotv * (1.0 - k) + k);
}
// NdotL : 法線ベクトルと光源へのベクトルとの内積
// NdotV : 法線ベクトルと視線へのベクトルとの内積
// roughness : 粗さ
float CalcGeometryFunction(float NdotL, float NdotV, float roughness)
{
    float r = roughness * 0.5f;
    float shadowing = CalcGeometryFunction(NdotL, r);
    float masking = CalcGeometryFunction(NdotV, r);
    return shadowing * masking;
}
//--------------------------------------------
// 鏡面反射BRDF（クック・トランスのマイクロファセットモデル）
//--------------------------------------------
// NdotV : 法線ベクトルと視線へのベクトルとの内積
// NdotL : 法線ベクトルと光源へのベクトルとの内積
// NdotH : 法線ベクトルとハーフベクトルとの内積
// VdotH : 視線へのベクトルとハーフベクトルとの内積
// fresnelF0 : 垂直入射時のフレネル反射色
// roughness : 粗さ
float3 SpecularBRDF(float NdotV, float NdotL, float NdotH, float VdotH, float3 fresnelF0, float roughness)
{
    // D項(法線分布)
    float D = CalcNormalDistributionFunction(NdotH, roughness);
    // G項(幾何減衰項)
    float G = CalcGeometryFunction(NdotL, NdotV, roughness);
    // F項(フレネル反射)
    float3 F = CalcFresnel(fresnelF0, VdotH);
    return (D * F * G) / max(0.0001f, 4 * NdotL * NdotV);
}


//--------------------------------------------
// 直接光の物理ベースライティング
//--------------------------------------------
// diffuseReflectance : 入射光のうち拡散反射になる割合
// F0 : 垂直入射時のフレネル反射色
// normal : 法線ベクトル(正規化済み)
// eyeVector : 視点に向かうベクトル(正規化済み)
// lightVector : 光源に向かうベクトル(正規化済み)
// lightColor : ライトカラー
// roughness : 粗さ
void DirectBDRF(float3 diffuseReflectance,
float3 F0,
float3 normal,
float3 eyeVector,
float3 lightVector,
float3 lightColor,
float roughness,
out float3 outDiffuse,
out float3 outSpecular)
{
    float3 N = normal;
    float3 L = -lightVector;
    float3 V = -eyeVector;
    float3 H = normalize(L + V);
    float NdotV = max(0.0001f, dot(N, V));
    float NdotL = max(0.0001f, dot(N, L));
    float NdotH = max(0.0001f, dot(N, H));
    float VdotH = max(0.0001f, dot(V, H));
    float3 irradiance = lightColor * NdotL;
    
    // 拡散反射BRDF
    outDiffuse = DiffuseBRDF(VdotH, F0, diffuseReflectance) * irradiance;
    // 鏡面反射BRDF
    outSpecular = (SpecularBRDF(NdotV, NdotL, NdotH, VdotH, F0, roughness) * irradiance);
}