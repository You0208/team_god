#include "lim_try.hlsli"

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
#define LINEAR_BORDER_BLACK 3
#define LINEAR_BORDER_WHITE 4

SamplerState sampler_states[3] : register(s0);
Texture2D texture_maps[8] : register(t0);

Texture2D noise_map : register(t9); // 先生ノイズは3D

Texture2D roughness_ : register(t4);
Texture2D metalness_ : register(t5);

// SHADOW
SamplerComparisonState comparison_sampler_state : register(s5);
Texture2D shadow_map : register(t8);

///////////////////////////////////////////
// シェーダーリソース
///////////////////////////////////////////
// モデルテクスチャ

///////////////////////////////////////////
// サンプラーステート
///////////////////////////////////////////
sampler g_sampler : register(s0);

///////////////////////////////////////////
// 関数宣言
///////////////////////////////////////////
float3 CalcLigFromDirectionLight(VS_OUT pin);

float4 main(VS_OUT pin) : SV_TARGET
{
    // ディレクションライトによるライティングを計算する
    float3 directionLig = CalcLigFromDirectionLight(pin);

    // step-6 サーフェイスに入射するスポットライトの光の向きを計算する
    // ピクセルの座標 - スポットライトの座標を計算
    float3 ligDir = pin.world_position - spot_light[0].position;

    // 正規化して大きさ1のベクトルにする
    ligDir = normalize(ligDir);
    
    // step-7 減衰なしのLambert拡散反射光を計算する
    float3 diffSpotLight = CalcLambertDiffuse(
        ligDir, // ライトの方向
        spot_light[0].color.rgb, // ライトのカラー
        pin.world_normal.rgb // サーフェイスの法線
    );
    // step-8 減衰なしのPhong鏡面反射光を計算する
    float3 specSpotLight = CalcPhongSpecular(
        ligDir, // ライトの方向
        spot_light[0].color.rgb, // ライトのカラー
        pin.world_position.rgb, // サーフェイスのワールド座標
        pin.world_normal.rgb     // サーフェイスの法線
    );
    
    // step-9 距離による影響率を計算する
    // スポットライトとの距離を計算する
    float3 distance = length(pin.world_position.rgb - spot_light[0].position.rgb);
    
    // 影響率は距離に比例して小さくなっていく
    float affect = 1.0f - 1.0f / spot_light[0].range* distance;
       // 影響力がマイナスにならないように補正をかける
    if (affect < 0.0f)
    {
        affect = 0.0f;
    }
    // 影響の仕方を指数関数的にする。今回のサンプルでは3乗している
    affect = pow(affect, 6.0f);
    
    // step-10 影響率を乗算して反射光を弱める
    diffSpotLight *= affect;
    specSpotLight *= affect;
    
    // step-11 入射光と射出方向の角度を求める
        // dot()を利用して内積を求める
    float angle = dot(ligDir, spot_light[0].direction.xyz);
    
    // dot()で求めた値をacos()に渡して角度を求める
    angle = abs(acos(angle));
    // step-12 角度による影響率を求める
    // 角度に比例して小さくなっていく影響率を計算する
    affect = 1.0f - 1.0f / spot_light[0].angle* angle;
        // 影響率がマイナスにならないように補正をかける
    if (affect < 0.0f)
    {
        affect = 0.0f;
    }
    // 影響の仕方を指数関数的にする。今回のサンプルでは0.5乗している
    affect = pow(affect, 0.5f);
    // step-13 角度による影響率を反射光に乗算して、影響を弱める
    diffSpotLight *= affect;
    specSpotLight *= affect;
    // ディレクションライト+ポイントライト+環境光を求める
    float3 finalLig = directionLig
                    + ambient_color.xyz;
    
    // step-14 スポットライトの反射光を最終的な反射光に足し算する
    finalLig += diffSpotLight + specSpotLight;
    
    // ディフューズマップをサンプリング
    float4 finalColor = texture_maps[0].Sample(sampler_states[ANISOTROPIC], pin.texcoord);

    // テクスチャカラーに求めた光を乗算して最終出力カラーを求める
    finalColor.xyz *= finalLig;
    
    return finalColor;
}

/// <summary>
/// </summary
/// ディレクションライトによる反射光を計算
/// <param name="psIn">ピクセルシェーダーからの入力。</param>
float3 CalcLigFromDirectionLight(VS_OUT pin)
{
    // ディレクションライトによるLambert拡散反射光を計算する
    float3 diffDirection = CalcLambertDiffuse(directional_light_direction.xyz, directional_light_color.xyz, pin.world_normal.rgb);

    // ディレクションライトによるPhong鏡面反射光を計算する
    float3 specDirection = CalcPhongSpecular(
            directional_light_direction.xyz, directional_light_color.xyz, pin.world_position.rgb, pin.world_normal.rgb);
    return diffDirection + specDirection;
}