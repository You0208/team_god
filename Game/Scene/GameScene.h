#pragma once
#include "./Lemur/Scene/BaseScene.h"
#include "./Lemur/Component/GameObject.h"
#include "./Lemur/Component/DemoPlayer.h"
#include "./Lemur/Component/DemoEnemy.h"
#include "./Lemur/Graphics/Shader.h"
#include "./Lemur/Graphics/Texture.h"
#include "./Lemur/Graphics/Framework.h"

// BLOOM
#include "./Lemur/Graphics/Bloom.h"

// Audio
#include <wrl.h>
#include "./Lemur/Audio/audio.h"

#include "./Lemur/Effekseer/Effect.h"

// Font
#include"./Lemur/Font/Font.h"
#include <dwrite.h>                
#pragma comment(lib,"d2d1.lib")    
#pragma comment(lib,"dwrite.lib")

// GLTF
#include "./Lemur/Model/GltfModel.h"

#include "./Lemur/Math/Vector.h"
#include "./Lemur/Math/MathHelper.h"

class GameScene :public Lemur::Scene::BaseScene
{
public:
    GameScene() {}
    ~GameScene() override {}

    // ������
    void Initialize()override;

    // �I����
    void Finalize()override;

    // �X�V����
    void Update(HWND hwnd, float elapsedTime)override;

    // �`�揈��
    void Render(float elapsedTime)override;

    // �f�o�b�N
    void DebugImgui()override;

private:

    //DemoPlayer
    DemoPlayer* player = nullptr;
    DemoEnemy* enemy = nullptr;

    float second;

    // skkind
    std::shared_ptr<SkinnedMesh> skinned_meshes[8];

    // gltf
    std::unique_ptr<GltfModel> gltf_models[8];

    // ADAPTER�p
    float adapter_timer;

    //TODO �����p
    DirectX::XMFLOAT3 pos = { 0.0f,0.0f,0.0f };

    DirectX::XMFLOAT4 camera_position{ 0.0f, 0.0f, 0.0f, 1.0f };
    DirectX::XMFLOAT3 camera_target{ 0.0f, 0.0f, 0.0f };
    float camera_range = 12.0f;
    //DirectX::XMFLOAT4 light_direction{ 0.0f, -1.0f, 1.0f, 1.0f };

    DirectX::XMFLOAT3 translation{ 0, 0, 0 };
    DirectX::XMFLOAT3 scaling{ 1, 1, 1 };
    DirectX::XMFLOAT3 rotation{ 0, 0, 0 };
    DirectX::XMFLOAT4 material_color{ 1, 1, 1, 1 };
private:
    std::unique_ptr<SpriteBatch> sprite_batches[8];

    std::unique_ptr<Framebuffer> framebuffers[8];
    enum class FRAME_BUFFER { SCENE, FOG };

    Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffers[8];
    enum class CONSTANT_BUFFER { SCENE, FOG, OPTION, PBR, D_FOG };
    enum class CONSTANT_BUFFER_R { NONE, SCENE, FOG, OPTION, PBR, D_FOG, LIGHT, HEMISPERE/*register�p*/ };

    std::unique_ptr<FullscreenQuad> fullscreenQuad;

    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shaders[8];
    enum class PS { FOG, FINAL, SKY };

    // shader
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_views[8];

    // dissolve
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> noise;

    // PBR
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> BaseColor;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Roughness;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Normal;

    // bloom
    std::unique_ptr<Bloom> bloomer;

    // skyMap
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> skymap;


    // MASK
    struct option_constants {
        DirectX::XMFLOAT4 parameters{ 1.0f,0.0f,0.0f,0.0f }; // x : �f�B�]���u�K���ʁAyzw : ��
    };
    option_constants option_constant;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mask_texture;
    std::shared_ptr<Sprite> dummy_sprite;
    Microsoft::WRL::ComPtr<ID3D11VertexShader> sprite_vertex_shader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> sprite_input_layout;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> sprite_pixel_shader;

    // �o�b�t�@�[���f�[�^���ꎞ�I�ɕێ����郁�����X�g���[�W�̈�
    // �E�v���O�������Œ萔�o�b�t�@�ɒl���i�[���邱�Ƃ�
    // �@�V�F�[�_�[���Ńv���O�������̃p�����[�^���Q�Ƃł���
    // �E���[���h�s��A�r���[�s��A�v���W�F�N�V�����s��Ȃǂ�
    // �@�p�����[�^��萔�o�b�t�@�ɏ������ނ��ƂŁA
    // �@�V�F�[�_�[���ō��W�ϊ��̌v�Z���ł���
    struct scene_constants
    {
        DirectX::XMFLOAT4X4 view_projection;// �r���[�s��
        //DirectX::XMFLOAT4 light_direction;// ���C�g����
        DirectX::XMFLOAT4 camera_position;// �J�����̈ʒu
        DirectX::XMFLOAT4X4 inverse_projection;// �t�s��
        float time;// ����
        float pads[3];
        //TODO ������ӂ����ƒ��ׂ�        
        DirectX::XMFLOAT4X4 inv_view_projection;// �t�r���[�s��

        // �e�p
        DirectX::XMFLOAT4X4 light_view_projection;// ���̃r���[�s��
        float shadow_depth_bias;
        float pads2[3];
    };
    scene_constants scene_constants;

    // FOG
    struct fog_constants
    {
        //float fog_color[4] = { 0.322f, 0.765f, 0.882f, 0.894f }; // w: fog intensuty
        float fog_color[4] = { 1.000f, 1.000f, 1.000f, 0.894f }; // w: fog intensuty
        float fog_density = 0.0007f;
        float fog_height_falloff = 0.9313f;
        float start_distance = 5.00f;
        float fog_cutoff_distance = 500.0f;
        float time_scale = 0.5f;
        float seed_scale = 0.2f;
        float pads[2];
    };
    fog_constants fog_constants;

    // STATIC_BATCHING
    size_t drawcall_count = 0;
    std::unique_ptr<StaticMeshBatch>  static_meshes[8];

    // �|�C���g���C�g
    struct pointLights
    {
        DirectX::XMFLOAT4 position{ 0, 0, 0, 0 };       // �ʒu
        DirectX::XMFLOAT4 color{ 1, 1, 1, 1 };          // �F
        float range{ 0 };                               // �͈�
        float pad[3];
    };

    // �X�|�b�g���C�g
    struct spotLights
    {
        DirectX::XMFLOAT4 position{ 0, 0, 0, 0 };       // �ʒu
        DirectX::XMFLOAT4 direction{ 0, 0, 1, 0 };      // ����
        DirectX::XMFLOAT4 color{ 1, 1, 1, 1 };          // �F
        float range{ 0 };                               // �͈�
        float pad[3];
        float angle;                                    // �p�x
        float pad2[3];
    };

    struct lightConstants
    {
        DirectX::XMFLOAT4 ambient_color;                // �A���r�G���g�J���[
        DirectX::XMFLOAT4 directional_light_direction;  // �f�B���N�V�������C�g�̕���
        DirectX::XMFLOAT4 directional_light_color;      // �f�B���N�V�������C�g�̐F

        pointLights point_light[8];                      // �|�C���g���C�g
        spotLights spot_light[8];                        // �X�|�b�g���C�g
    };
    Microsoft::WRL::ComPtr<ID3D11Buffer> light_constant_buffer;
    lightConstants lights{};

    DirectX::XMFLOAT4 ambient_color{ 0.2f, 0.2f, 0.2f, 0.2f };
    DirectX::XMFLOAT4 directional_light_direction{ 1.0f, -1.0f, 1.0f, 1.0f };
    DirectX::XMFLOAT4 directional_light_color{ 1.0f, 1.0f, 1.0f, 1.0f };
    pointLights point_light[8];
    spotLights spot_light[8];
    float sp_angle = 0;

    Vector3 spDirection = {1,1,1};

    // PBR
    struct adjust_constants
    {
        DirectX::XMFLOAT4 materialColor;
        DirectX::XMFLOAT4 adjustMetalness = { 0.0f,0,0,0 }; // ���^���l�X�̒����l
        DirectX::XMFLOAT4 adjustSmoothness = { 0.0f,0,0,0 }; // ���炩���̒����l
    };
    adjust_constants pbr;

    // �����t�H�O
    struct dis_fog_constants
    {
        DirectX::XMFLOAT4 fog_color;
        DirectX::XMFLOAT4 fog_range; // x : near, y : far, zw : ��
    };
    DirectX::XMFLOAT4 fog_color{ 0.2f, 0.2f, 0.2f, 1.0f }; // �N���A�J���[�ɂ��Ă��܂�
    DirectX::XMFLOAT4 fog_range{ 0.1f, 100.0f, 0, 0 };

    // �������C�g
    struct hemisphere_light_constants
    {
        DirectX::XMFLOAT4 sky_color;
        DirectX::XMFLOAT4 ground_color;
        DirectX::XMFLOAT4 hemisphere_weight; // x : weight, yzw : ��
    };
    Microsoft::WRL::ComPtr<ID3D11Buffer> hemisphere_light_constant_buffer;
    DirectX::XMFLOAT4 sky_color{ 1.0f, 0.0f, 0.0f, 1.0f };
    DirectX::XMFLOAT4 ground_color{ 0.0f, 0.0f, 1.0f, 1.0f };
    float hemisphere_weight{ 0.0f };


    // SHADOW
    const uint32_t shadowmap_width = 2048;
    const uint32_t shadowmap_height = 2048;
    std::unique_ptr<ShadowMap> double_speed_z;
    DirectX::XMFLOAT4 light_view_focus{ 0, 0, 0, 1 };
    float light_view_distance{ 10.0f };
    float light_view_size{ 12.0f };
    float light_view_near_z{ 2.0f };
    float light_view_far_z{ 18.0f };

    // �R���g���[���[�p
    float s_l;
    float timer_s;
    float s_l_max;
    float f_d=0;
    std::shared_ptr<Sprite> ohajiki;


    
};