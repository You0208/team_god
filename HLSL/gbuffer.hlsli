
// �s�N�Z���V�F�[�_�[�ւ̏o�͗p�\����
struct PSGBufferOut
{
    float4 base_color : SV_TARGET0;
    float4 emissive_color : SV_TARGET1;
    float4 normal : SV_TARGET2;
    float4 parameter : SV_TARGET3;
    float depth : SV_TARGET4;
};

// GBuffer���\����
struct GBufferData
{
    float4 base_color; // �x�[�X�J���[
    float3 emissive_color; // ���Ȕ����F
    float3 w_normal; // ���[���h�@��
    float3 w_position; // ���[���h���W(Decode���̂�)
    float depth; // �[�x
    float roughness; // �e��
    float metalness; // ������
    float occlusion_factor; // �Օ��l
    float occlusion_strength; // �Օ����x
};

// GBuffer�e�N�X�`���󂯓n���p�\����
struct PSGBufferTextures
{
    Texture2D<float4> base_color;
    Texture2D<float4> emissive_color;
    Texture2D<float4> normal;
    Texture2D<float4> parameter;
    Texture2D<float> depth;
    SamplerState state;
};

// GBufferData�ɓZ�߂������s�N�Z���V�F�[�_�[�̏o�͗p�\���̂ɕϊ�
PSGBufferOut EncodeGBuffer(in GBufferData data, matrix view_projection_matrix)
{
    PSGBufferOut ret = (PSGBufferOut) 0;
    
    // �x�[�X�J���[��base_color(RT0)�ɏo��
    ret.base_color = data.base_color;
    // ���Ȕ����F��emissive_color(RT1)�ɏo��
    ret.emissive_color.rgb = data.emissive_color;
    ret.emissive_color.a = 1;
    // �@����normal(RT2)�ɏo��
    ret.normal.rgb = data.w_normal;
    ret.normal.a = 1;
    // �e��/������/�Օ���parameter(RT3)�ɏo��
    ret.parameter.r = data.occlusion_factor;
    ret.parameter.g = data.roughness;
    ret.parameter.b = data.metalness;
    ret.parameter.a = data.occlusion_strength;
    // �[�x��depth(RT4)�ɏo��
    float4 position = mul(float4(data.w_position, 1.0f), view_projection_matrix);
    ret.depth = position.z / position.w;
    return ret;
};

// �s�N�Z���V�F�[�_�[�̏o�͗p�\���̂���GBufferData���ɕϊ�
GBufferData DecodeGBuffer(PSGBufferTextures textures, float2 uv, matrix inverse_view_projection_matrix)
{
    // �e�e�N�X�`����������擾
    float4 base_color = textures.base_color.Sample(textures.state, uv);
    float4 emissive_color = textures.emissive_color.Sample(textures.state, uv);
    float4 normal = textures.normal.Sample(textures.state, uv);
    float4 parameter = textures.parameter.Sample(textures.state, uv);
    float depth = textures.depth.Sample(textures.state, uv);
    GBufferData ret = (GBufferData) 0;
    
    // base_color.rgb�Ƀx�[�X�J���[
    ret.base_color = base_color;
    
    // emissive_color�ɂ͎��Ȕ����F
    ret.emissive_color = emissive_color.rgb;
    
    // normal�ɖ@��
    ret.w_normal = normal.rgb;
    
    // parameter����e��/������/�Օ�
    ret.occlusion_factor = parameter.r;
    ret.roughness = parameter.g;
    ret.metalness = parameter.b;
    ret.occlusion_strength = parameter.a;
    
    // depth����[�x
    ret.depth = depth;
    
    // �[�x, UV, inverse_view_projection_matrix���烏�[���h���W�𕜌�
    float4 position = float4(uv.x * 2.0f - 1.0f, uv.y * -2.0f + 1.0f, depth, 1);
    position = mul(position, inverse_view_projection_matrix);
    ret.w_position = position.xyz / position.w;
    
    return ret;
}