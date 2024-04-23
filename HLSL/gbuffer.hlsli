
// ピクセルシェーダーへの出力用構造体
struct PSGBufferOut
{
    float4 base_color : SV_TARGET0;
    float4 emissive_color : SV_TARGET1;
    float4 normal : SV_TARGET2;
    float4 parameter : SV_TARGET3;
    float depth : SV_TARGET4;
};

// GBuffer情報構造体
struct GBufferData
{
    float4 base_color; // ベースカラー
    float3 emissive_color; // 自己発光色
    float3 w_normal; // ワールド法線
    float3 w_position; // ワールド座標(Decode時のみ)
    float depth; // 深度
    float roughness; // 粗さ
    float metalness; // 金属質
    float occlusion_factor; // 遮蔽値
    float occlusion_strength; // 遮蔽強度
};

// GBufferテクスチャ受け渡し用構造体
struct PSGBufferTextures
{
    Texture2D<float4> base_color;
    Texture2D<float4> emissive_color;
    Texture2D<float4> normal;
    Texture2D<float4> parameter;
    Texture2D<float> depth;
    SamplerState state;
};

// GBufferDataに纏めた情報をピクセルシェーダーの出力用構造体に変換
PSGBufferOut EncodeGBuffer(in GBufferData data, matrix view_projection_matrix)
{
    PSGBufferOut ret = (PSGBufferOut) 0;
    
    // ベースカラーはbase_color(RT0)に出力
    ret.base_color = data.base_color;
    // 自己発光色はemissive_color(RT1)に出力
    ret.emissive_color.rgb = data.emissive_color;
    ret.emissive_color.a = 1;
    // 法線はnormal(RT2)に出力
    ret.normal.rgb = data.w_normal;
    ret.normal.a = 1;
    // 粗さ/金属質/遮蔽はparameter(RT3)に出力
    ret.parameter.r = data.occlusion_factor;
    ret.parameter.g = data.roughness;
    ret.parameter.b = data.metalness;
    ret.parameter.a = data.occlusion_strength;
    // 深度はdepth(RT4)に出力
    float4 position = mul(float4(data.w_position, 1.0f), view_projection_matrix);
    ret.depth = position.z / position.w;
    return ret;
};

// ピクセルシェーダーの出力用構造体からGBufferData情報に変換
GBufferData DecodeGBuffer(PSGBufferTextures textures, float2 uv, matrix inverse_view_projection_matrix)
{
    // 各テクスチャから情報を取得
    float4 base_color = textures.base_color.Sample(textures.state, uv);
    float4 emissive_color = textures.emissive_color.Sample(textures.state, uv);
    float4 normal = textures.normal.Sample(textures.state, uv);
    float4 parameter = textures.parameter.Sample(textures.state, uv);
    float depth = textures.depth.Sample(textures.state, uv);
    GBufferData ret = (GBufferData) 0;
    
    // base_color.rgbにベースカラー
    ret.base_color = base_color;
    
    // emissive_colorには自己発光色
    ret.emissive_color = emissive_color.rgb;
    
    // normalに法線
    ret.w_normal = normal.rgb;
    
    // parameterから粗さ/金属質/遮蔽
    ret.occlusion_factor = parameter.r;
    ret.roughness = parameter.g;
    ret.metalness = parameter.b;
    ret.occlusion_strength = parameter.a;
    
    // depthから深度
    ret.depth = depth;
    
    // 深度, UV, inverse_view_projection_matrixからワールド座標を復元
    float4 position = float4(uv.x * 2.0f - 1.0f, uv.y * -2.0f + 1.0f, depth, 1);
    position = mul(position, inverse_view_projection_matrix);
    ret.w_position = position.xyz / position.w;
    
    return ret;
}