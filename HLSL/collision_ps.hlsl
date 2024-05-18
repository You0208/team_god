
#include "light.hlsli"
Texture2D Edge : register(t11);

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
#define LINEAR_BORDER_BLACK 3
#define LINEAR_BORDER_WHITE 4

SamplerState sampler_states[3] : register(s0);
float4 main(VS_OUT pin) : SV_TARGET
{
     
//-----------------------------------------
//　輪郭線
//----------------------------------------- 
#if 1
    // 正規化スクリーン座標系からUV座標系に変換する
    float2 uv = pin.posInProj.xy * float2(0.5f, -0.5f) + 0.5f;
        
        // 近傍8テクセルへのUVオフセット
    float2 uvOffset[8] =
    {
        float2(0.0f, 1.0f / 720.0f), //上
        float2(0.0f, -1.0f / 720.0f), //下
        float2(1.0f / 1280.0f, 0.0f), //右
        float2(-1.0f / 1280.0f, 0.0f), //左
        float2(1.0f / 1280.0f, 1.0f / 720.0f), //右上
        float2(-1.0f / 1280.0f, 1.0f / 720.0f), //左上
        float2(1.0f / 1280.0f, -1.0f / 720.0f), //右下
        float2(-1.0f / 1280.0f, -1.0f / 720.0f) //左下
    };

    // このピクセルの深度値を取得
    float depth = Edge.Sample(sampler_states[ANISOTROPIC], uv).x;

    // 近傍8テクセルの深度値の平均値を計算する
    float depth2 = 0.0f;
    for (int i = 0; i < 8; i++)
    {
        depth2 += Edge.Sample(sampler_states[ANISOTROPIC], uv + uvOffset[i]).x;
    }
    depth2 /= 8.0f;

    // 自身の深度値と近傍8テクセルの深度値の差を調べる
    if (abs(depth - depth2) > 0.00005f)
    {
        // 深度値が結構違う場合はピクセルカラーを黒にする
        // ->これがエッジカラーとなる
        return float4(parameters.x, parameters.y, parameters.w, 0.6f);
    }
    
#endif
    
    
    //return float4(1.0f, 1.0f, 0.9f, 0.2f);// ゼルダホワイト
    return float4(1.0f, 0.5f, 0.0f, 0.1f);
}