
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

////TODO GLTF�p������
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


// �֐��錾
void DirectBDRF(float3 diffuseReflectance, float3 F0, float3 normal, float3 eyeVector, float3 lightVector, float3 lightColor, float roughness, out float3 outDiffuse, out float3 outSpecular);


// Lambert�g�U���ˌ����v�Z����
float3 CalcLambertDiffuse(float3 lightDirection, float3 lightColor, float3 normal)
{
    // �s�N�Z���̖@���ƃ��C�g�̕����̓��ς��v�Z����
    float t = dot(normal, lightDirection) * -1.0f;

    // ���ς̒l��0�ȏ�̒l�ɂ���
    t = max(0.0f, t);

    // �g�U���ˌ����v�Z����
    return lightColor * t;
}

// Phong���ʔ��ˌ����v�Z����
float3 CalcPhongSpecular(float3 lightDirection, float3 lightColor, float3 worldPos, float3 normal)
{
    // ���˃x�N�g�������߂�
    float3 refVec = reflect(lightDirection, normal);

    // �������������T�[�t�F�C�X���王�_�ɐL�т�x�N�g�������߂�
    float3 toEye = camera_position.xyz - worldPos;
    toEye = normalize(toEye);

    // ���ʔ��˂̋��������߂�
    float t = dot(refVec, toEye);

    // ���ʔ��˂̋�����0�ȏ�̐��l�ɂ���
    t = max(0.0f, t);

    // ���ʔ��˂̋������i��
    t = pow(t, 5.0f);

    // ���ʔ��ˌ������߂�
    return lightColor * t;
}

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

//-----------------------PBR���C�e�B���O------------------------------------------------

// �|�C���g���C�g
void PointLight(VS_OUT pin,
float3 diffuseReflectance,
float3 F0,
float3 N,
float3 E,
float roughness,
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
        
        float3 diffuse = 0, specular = 0;
        DirectBDRF(diffuseReflectance, F0, N, E, lightVector,
            point_light[i].color.rgb, roughness,
            diffuse, specular);
        
        outDiffuse = diffuse * attenuate;
        outSpecular = specular * attenuate;
    }
}

// �X�|�b�g���C�g
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
    for (int j = 0; j < 1; ++j)
    {
        // �T�[�t�F�C�X�ɓ��˂���X�|�b�g���C�g�̌��̌������v�Z����
        float3 ligDir = pin.w_position.xyz - spot_light[j].position.xyz;
        // �X�|�b�g���C�g�̃J���[
        float3 spColor = spot_light[j].color.xyz;
        // ���K�����đ傫��1�̃x�N�g���ɂ���
        ligDir = normalize(ligDir);
        
        float3 diffSpotLight = 0;
        float3 specSpotLight = 0;

        DirectBDRF(diffuseReflectance, F0, N, E, ligDir,
            spot_light[j].color.rgb, roughness,
            diffSpotLight, specSpotLight);
        
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
        float angle = dot(ligDir, spot_light[j].direction.xyz);
        
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

//------------------------- PBR���i--------------------------------------------------

// �U�d��(������ł��Œ�4%(0.0f4)�͋��ʔ��˂���)
static const float Dielectric = 0.04f;
//--------------------------------------------
// �g�U����BRDF(���K�������o�[�g�̊g�U����)
//--------------------------------------------
// diffuseReflectance : ���ˌ��̂����g�U���˂ɂȂ銄��
float3 DiffuseBRDF(float3 diffuseReflectance)
{
    return diffuseReflectance / PI;
}

//--------------------------------------------
// �t���l����
//--------------------------------------------
// F0 : �������ˎ��̔��˗�
// VdotH: �����x�N�g���ƃn�[�t�x�N�g���i�����ւ̃x�N�g���Ǝ��_�ւ̃x�N�g���̒��ԃx�N�g��
float3 CalcFresnel(float3 F0, float VdotH)
{
    return F0 + (1.0f - F0) * pow(1.0f - VdotH, 5.0f);
}
//--------------------------------------------
// �@�����z�֐�
//--------------------------------------------
// NdotH : �@���x�N�g���ƃn�[�t�x�N�g���i�����ւ̃x�N�g���Ǝ��_�ւ̃x�N�g���̒��ԃx�N�g���j�̓���
// roughness : �e��
float CalcNormalDistributionFunction(float NdotH, float roughness)
{
    float alpha = roughness * roughness;
    float alpha2 = alpha * alpha;
    float denom = (NdotH * NdotH) * (alpha2 - 1.0f) + 1.0f;
    return alpha2 / (PI * denom * denom);
}
//--------------------------------------------
// �􉽌������̎Z�o
//--------------------------------------------
// Ndotv : �@���Ƒ��x�N�g���Ƃ̓���
// k : �e��/2
float CalcGeometryFunction(float Ndotv, float k)
{
    return Ndotv / (Ndotv * (1.0 - k) + k);
}
// NdotL : �@���x�N�g���ƌ����ւ̃x�N�g���Ƃ̓���
// NdotV : �@���x�N�g���Ǝ����ւ̃x�N�g���Ƃ̓���
// roughness : �e��
float CalcGeometryFunction(float NdotL, float NdotV, float roughness)
{
    float r = roughness * 0.5f;
    float shadowing = CalcGeometryFunction(NdotL, r);
    float masking = CalcGeometryFunction(NdotV, r);
    return shadowing * masking;
}
//--------------------------------------------
// ���ʔ���BRDF�i�N�b�N�E�g�����X�̃}�C�N���t�@�Z�b�g���f���j
//--------------------------------------------
// NdotV : �@���x�N�g���Ǝ����ւ̃x�N�g���Ƃ̓���
// NdotL : �@���x�N�g���ƌ����ւ̃x�N�g���Ƃ̓���
// NdotH : �@���x�N�g���ƃn�[�t�x�N�g���Ƃ̓���
// VdotH : �����ւ̃x�N�g���ƃn�[�t�x�N�g���Ƃ̓���
// fresnelF0 : �������ˎ��̃t���l�����ːF
// roughness : �e��
float3 SpecularBRDF(float NdotV, float NdotL, float NdotH, float VdotH, float3 fresnelF0, float roughness)
{
 // D��(�@�����z)
    float D = CalcNormalDistributionFunction(NdotH, roughness);
 // G��(�􉽌�����)
    float G = CalcGeometryFunction(NdotL, NdotV, roughness);
 // F��(�t���l������)
    float3 F = CalcFresnel(fresnelF0, VdotH);
    return (D * F * G) / max(0.0001f, 4 * NdotL * NdotV);
}

//--------------------------------------------
// ���ڌ��̕����x�[�X���C�e�B���O
//--------------------------------------------
// diffuseReflectance : ���ˌ��̂����g�U���˂ɂȂ銄��
// F0 : �������ˎ��̃t���l�����ːF
// normal : �@���x�N�g��(���K���ς�)
// eyeVector : ���_�Ɍ������x�N�g��(���K���ς�)
// lightVector : �����Ɍ������x�N�g��(���K���ς�)
// lightColor : ���C�g�J���[
// roughness : �e��
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
    float3 V = eyeVector;
    float3 H = normalize(L + V);
    float NdotV = max(0.0001f, dot(N, V));
    float NdotL = max(0.0001f, dot(N, L));
    float NdotH = max(0.0001f, dot(N, H));
    float VdotH = max(0.0001f, dot(V, H));
    float3 irradiance = lightColor * NdotL;
// ���K�V�[�ȃ��C�e�B���O�Ƃ̌݊�����ۂꍇ��PI�ŏ�Z����
    irradiance *= PI;
// �g�U����BRDF(���K�������o�[�g�̊g�U����)
    outDiffuse = DiffuseBRDF(diffuseReflectance) * irradiance;
// ���ʔ���BRDF�i�N�b�N�E�g�����X�̃}�C�N���t�@�Z�b�g���f���j
    outSpecular = SpecularBRDF(NdotV, NdotL, NdotH, VdotH, F0, roughness) * irradiance;
}
