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

// ���C�g�p
struct point_lights
{
    float4 position;
    float4 color;
    float range;
    float3 dummy;
};
// �X�|�b�g���C�g
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

// �~����
static const float PI = 3.141592654f;
static const int MAX_BONES = 256;
// �������C�g
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

// ���C�g�f�[�^�ɃA�N�Z�X���邽�߂̒萔�o�b�t�@�[��p�ӂ���
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


// �t�H�O
float4 CalcFog(in float4 color, float4 fog_color, float2 fog_range, float eye_length)
{
    float fogAlpha = saturate((eye_length - fog_range.x) / (fog_range.y - fog_range.x));
    return lerp(color, fog_color, fogAlpha);
}


//-----------------------�V���h�E------------------------------------------------

void Shadow(VS_OUT pin, float3 normal, float3 T, float3 B,
Texture2D shadow_map, SamplerComparisonState comparison_sampler_state, out float3 shadow_factor)
{
    float3 L = normalize(-directional_light_direction.xyz);
    float3 SN = normalize(pin.w_normal.xyz);
    // �ڃx�N�g��        
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

//-----------------------���C�e�B���O���̑�------------------------------------------------

//--------------------------------------------
// �������C�g
//--------------------------------------------
// N:�@��(���K���ς�)
// E:���_�����x�N�g��(���K���ς�)
// L:���˃x�N�g��(���K���ς�)
// C :���C�g�F
// RimPower : �������C�g�̋���(�����l�̓e�L�g�[�Ȃ̂Ŏ����Őݒ肷�邪�g)
float3 CalcRimLight(float3 normal, float3 E, float3 L, float3 lightColor, float RimPower = 3.0f)
{
    float rim = 1.0f - saturate(dot(normal, -E));
    return lightColor * pow(rim, RimPower) * saturate(dot(L, -E));
}

//--------------------------------------------
// �������C�e�B���O
//--------------------------------------------
// normal:�@��(���K���ς�)
// up:������i�Е��j
// sky_color:��(��)�F
// ground_color:�n��(��)�F
// hemisphere_weight:�d��
float3 CalcHemiSphereLight(float3 normal, float3 up, float3 sky_color, float3 ground_color, float4 hemisphere_weight)
{
    float factor = dot(normal, up) * 0.5f + 0.5f;
    return lerp(ground_color, sky_color, factor) * hemisphere_weight.x;
}

// �������C�g
void LimHemiLight(VS_OUT pin,
float3 normal,
float3 N_,
float3 T,
float3 E,
out float3 rimColor,
out float3 hemiLight
)
{
    // ���C�g�����x�N�g��
    float3 Light = normalize(directional_light_direction.xyz);
     // �e�x�N�g���𐳋K�����A�������s��̊e��Ɋ��蓖��
    float3x3 mat = { T, pin.binormal, N_ };
     // �m�[�}���e�N�X�`���@�������[���h�֕ϊ�
    normal = normalize(mul(normal * 2.0f - 1.0f, mat));
     
    rimColor = CalcRimLight(normal, -E, Light, directional_light_color.rgb);
     // �������C�g
    hemiLight = CalcHemiSphereLight(normal, float3(0, 1, 0), sky_color.rgb, ground_color.rgb, hemisphere_weight);
    
}


/// <summary>
/// Lambert�g�U���ˌ����v�Z����
/// </summary>
float3 CalcLambertDiffuse(float3 lightDirection, float3 lightColor, float3 normal)
{
    // �s�N�Z���̖@���ƃ��C�g�̕����̓��ς��v�Z����
    float t = dot(normal, lightDirection) * -1.0f;

    // ���ς̒l��0�ȏ�̒l�ɂ���
    t = max(0.0f, t);

    // �g�U���ˌ����v�Z����
    return lightColor * t;
}

//-----------------------PBR���C�e�B���O------------------------------------------------

// �|�C���g���C�g
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
        // �T�[�t�F�C�X�ɓ��˂��郉�C�g�̌��̌������v�Z����
        float3 lightVector = pin.w_position.xyz - point_light[i].position.xyz;
        float lightLength = length(lightVector);
        if (lightLength >= point_light[i].range)
            continue;
        float attenuate = saturate(1.0f - lightLength / point_light[i].range);
        lightVector = lightVector / lightLength;
        
        // �|�C���g���C�g�Ƃ̋������v�Z����
        float3 distance = length(pin.w_position.xyz - point_light[i].position.xyz);
        // �e�����͋����ɔ�Ⴕ�ď������Ȃ��Ă���
        float affect = 1.0f - 1.0f / point_light[i].range.x * distance;
        // �e���͂��}�C�i�X�ɂȂ�Ȃ��悤�ɕ␳��������
        if (affect < 0.0f)
        {
            affect = 0.0f;
        }
        //�e�����w���֐��I�ɂ���B����̃T���v���ł�3�悵�Ă���
        affect = pow(affect, 3.0f);
        
        float3 diffuse = 0;
        float3 specular = 0;
    
        // ���C�g���Ƃ̃V�F�[�f�B���O�����̃��[�v
        float3 L = lightVector;
        float3 Li = point_light[i].color; // ���̋P��
    
        // �@���x�N�g���ƌ����ւ̃x�N�g���Ƃ̓���
        const float NoL = max(0.0, dot(N, L));
        // �@���x�N�g���Ǝ����ւ̃x�N�g���Ƃ̓���
        const float NoV = max(0.0, dot(N, V));
    
        // ���ς�0����Ȃ��Ȃ�i�����łȂ��Ȃ�j
        if (NoL > 0.0 || NoV > 0.0)
        {
            const float3 R = reflect(-L, N); // ���˃x�N�g��
            const float3 H = normalize(V + L); // �n�[�t�x�N�g���i�����ւ̃x�N�g���Ǝ��_�ւ̃x�N�g���̒��ԃx�N�g���j
        
            const float NoH = max(0.0, dot(N, H)); // �@���x�N�g���ƃn�[�t�x�N�g���̓���
            const float HoV = max(0.0, dot(H, V)); // VoH: �����x�N�g���ƃn�[�t�x�N�g���̓���
        
            diffuse += Li * NoL * brdf_lambertian(f0, f90, c_diff, HoV);
            specular += Li * NoL * brdf_specular_ggx(f0, f90, alpha_roughness, HoV, NoL, NoV, NoH);
        }
        
        outDiffuse = diffuse * attenuate;
        outSpecular = specular * attenuate;
    }
}

// �X�|�b�g���C�g
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
        // �T�[�t�F�C�X�ɓ��˂���X�|�b�g���C�g�̌��̌������v�Z����
        float3 L =spot_light[j].direction.xyz;
        // �X�|�b�g���C�g�̃J���[
        float3 Li = spot_light[j].color.xyz;
       // outDiffuse = L;
           // �@���x�N�g���ƌ����ւ̃x�N�g���Ƃ̓���
        const float NoL = max(0.0, dot(N, L));
        // �@���x�N�g���Ǝ����ւ̃x�N�g���Ƃ̓���
        const float NoV = max(0.0, dot(N, V));
    
        // ���ς�0����Ȃ��Ȃ�i�����łȂ��Ȃ�j
        if (NoL > 0.0 || NoV > 0.0)
        {
            const float3 R = reflect(-L, N); // ���˃x�N�g��
            const float3 H = normalize(V + L); // �n�[�t�x�N�g���i�����ւ̃x�N�g���Ǝ��_�ւ̃x�N�g���̒��ԃx�N�g���j
        
            const float NoH = max(0.0, dot(N, H)); // �@���x�N�g���ƃn�[�t�x�N�g���̓���
            const float HoV = max(0.0, dot(H, V)); // VoH: �����x�N�g���ƃn�[�t�x�N�g���̓���
        
            diffSpotLight += Li * NoL * brdf_lambertian(f0, f90, c_diff, HoV);
            specSpotLight += Li * NoL * brdf_specular_ggx(f0, f90, alpha_roughness, HoV, NoL, NoV, NoH);
        }
        
        // ���K�����đ傫��1�̃x�N�g���ɂ���
        L = normalize(L);
        
        // �����ɂ��e�������v�Z����
        // �X�|�b�g���C�g�Ƃ̋������v�Z����
        float3 distance = length(pin.w_position.xyz - spot_light[j].position.xyz);

        // �e�����͋����ɔ�Ⴕ�ď������Ȃ��Ă���
        float affect = 1.0f - 1.0f / spot_light[j].range * distance;
        
        // �e���͂��}�C�i�X�ɂȂ�Ȃ��悤�ɕ␳��������
        if (affect < 0.0f)
        {
            affect = 0.0f;
        }

        // �e���̎d�����w���֐��I�ɂ���
        affect = pow(affect, 6.0f);
        
        // �e��������Z���Ĕ��ˌ�����߂�
        diffSpotLight *= affect;
        specSpotLight *= affect;
        
        // ���ˌ��Ǝˏo�����̊p�x�����߂�
        // dot()�𗘗p���ē��ς����߂�
        float angle = dot(L, spot_light[j].direction.xyz);
        
        // dot()�ŋ��߂��l��acos()�ɓn���Ċp�x�����߂�
        angle = abs(acos(angle));
        
        // �p�x�ɂ��e���������߂�
        // �p�x�ɔ�Ⴕ�ď������Ȃ��Ă����e�������v�Z����
        affect = 1.0f - 1.0f / spot_light[j].angle * angle;

        // �e�������}�C�i�X�ɂȂ�Ȃ��悤�ɕ␳��������
        if (affect < 0.0f)
        {
            affect = 0.0f;
        }
        
        // �e���̎d�����w���֐��I�ɂ���B����̃T���v���ł�0.5�悵�Ă���
        affect = pow(affect, 0.5f);

        // �p�x�ɂ��e�����𔽎ˌ��ɏ�Z���āA�e������߂�
        diffSpotLight *= affect;
        specSpotLight *= affect;
        
        outDiffuse = diffSpotLight;
        outSpecular = specSpotLight;
    }
}