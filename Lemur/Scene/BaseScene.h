#pragma once

#include "../Graphics/Graphics.h"
#include <windows.h>
#include <sstream>
#include <d3d11.h>
#include <wrl.h>

#include "../Math/MathHelper.h"
#include "../Math/Vector.h"

#include "../Graphics/Shader.h"

#include "../Input/Input.h"
#include "../imgui/ImGuiCtrl.h"

namespace Lemur::Scene
{
    // �x�[�X�V�[��
    class BaseScene
    {
    public:
        BaseScene() {}
        virtual ~BaseScene() {}

        // ������
        virtual void Initialize() = 0;
        // �I����
        virtual void Finalize() = 0;
        // �X�V����
        virtual void Update(HWND hwnd, float elapsefTime) = 0;
        // �`�揈��
        virtual void Render(float elapsedTime) = 0;
        // �f�o�b�O
        virtual void DebugImgui();
        // �����������Ă��邩
        bool IsReady()const { return ready; }
        // ���������ݒ�
        void SetReady() { ready = true; }

    protected:
        // �X�e�[�g�̏�����
        void InitializeState();

        // �����_�����O�O�ݒ�
        void SetUpRendering();

        // �f�B�t�@�[�h�����_�����O�̏�����
        void InitializeDeffered(int textureWidth, int textureHeight);

        // �f�B�t�@�[�h�����_�����O�̃Z�b�g
        void SetUpDeffered();

        // �f�B�t�@�[�h�����_�����O�̕`��
        void RenderingDeffered();

        // ���C�g�̃Z�b�g
        virtual void InitializeLight();

        // �o�b�t�@�[�̏�����
        void InitializeFramebuffer();

        // �s�N�Z���V�F�[�_�[�̏�����
        void InitializePS();

        // �|�X�g�G�t�F�N�g�̎��s
        void ExePostEffct();

        void LightUpdate();
    protected:
        // �T���v���[�X�e�[�g
        enum class SAMPLER_STATE { POINT, LINEAR, ANISOTROPIC, LINEAR_BORDER_BLACK, LINEAR_BORDER_WHITE, COMPARISON_LINEAR_BORDER_WHITE, LINEAR_CLAMP };
        Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_states[7];

        // �[�x�X�e�[�g
        enum  DEPTH_STATE { ZT_ON_ZW_ON, ZT_ON_ZW_OFF, ZT_OFF_ZW_ON, ZT_OFF_ZW_OFF };
        Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depth_stencil_states[4];

        // �u�����h�X�e�[�g
        enum  BLEND_STATE { NONE, ALPHA, ADD, MULTIPLY, MLT_ALPHA };
        Microsoft::WRL::ComPtr<ID3D11BlendState> blend_states[5];

        // ���C�X�^���C�U�X�e�[�g
        enum class RASTER_STATE { SOLID, WIREFRAME, CULL_NONE, WIREFRAME_CULL_NONE };
        Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer_states[4];

        // �s�N�Z���V�F�[�_�[
        Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shaders[8];
        enum class PS { FOG, FINAL, SKY, DEFFERED };

        // �R���X�^���g�o�b�t�@�[
        Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffers[8];
        enum class CONSTANT_BUFFER { SCENE, FOG, OPTION, PBR, D_FOG };
        enum class CONSTANT_BUFFER_R { NONE, SCENE, FOG, OPTION, PBR, D_FOG, LIGHT, HEMISPERE/*register�p*/ };

        std::unique_ptr<Framebuffer> framebuffers[8];
        enum class FRAME_BUFFER { SCENE, FOG, DEPTH,TEX };

        // MASK
        struct option_constants {
            DirectX::XMFLOAT4 parameters{ 0.0f,0.0f,0.0f,0.0f }; // x : �f�B�]���u�K���ʁAyzw : ��
        };
        option_constants option_constant;


        // �o�b�t�@�[���f�[�^���ꎞ�I�ɕێ����郁�����X�g���[�W�̈�
        // �E�v���O�������Œ萔�o�b�t�@�ɒl���i�[���邱�Ƃ�
        // �@�V�F�[�_�[���Ńv���O�������̃p�����[�^���Q�Ƃł���
        // �E���[���h�s��A�r���[�s��A�v���W�F�N�V�����s��Ȃǂ�
        // �@�p�����[�^��萔�o�b�t�@�ɏ������ނ��ƂŁA
        // �@�V�F�[�_�[���ō��W�ϊ��̌v�Z���ł���
        struct scene_constants
        {
            DirectX::XMFLOAT4X4 view_projection;// �r���[�s��
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

        Vector3 spDirection;

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

        // �J����
        DirectX::XMFLOAT4 camera_position{ 0.0f, 0.0f, 0.0f, 1.0f };
        DirectX::XMFLOAT3 camera_target{ 0.0f, 0.0f, 0.0f };
        DirectX::XMFLOAT3 camera_angle{ 0.0f, 0.0f, 0.0f };
        float camera_range = 12.0f;

        // �f�B�t�@�[�h�����_�����O
        enum GBUFFER { BASECOLOR, NORMAL, POSITION, MS, EMISSIVE, OCCLUSION, G_MAX };

        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceViewArray[G_MAX];
        Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetViewArray[G_MAX];


        // bloom
        std::unique_ptr<Bloom> bloomer;
        // ��ʗp
        std::unique_ptr<FullscreenQuad> fullscreenQuad;
    protected:
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mask_texture;
        std::shared_ptr<Sprite> dummy_sprite;
        Microsoft::WRL::ComPtr<ID3D11VertexShader> sprite_vertex_shader;
        Microsoft::WRL::ComPtr<ID3D11InputLayout> sprite_input_layout;
        Microsoft::WRL::ComPtr<ID3D11PixelShader> sprite_pixel_shader;

        // Zelda_Shader
        Microsoft::WRL::ComPtr<ID3D11PixelShader> zelda_ps;
        Microsoft::WRL::ComPtr<ID3D11PixelShader> Wall;
        Microsoft::WRL::ComPtr<ID3D11PixelShader> Try;
        Microsoft::WRL::ComPtr<ID3D11PixelShader> chara_ps;
        Microsoft::WRL::ComPtr<ID3D11PixelShader> stage_ps;

        Microsoft::WRL::ComPtr<ID3D11PixelShader> fbx_gbuffer_ps;
        Microsoft::WRL::ComPtr<ID3D11PixelShader> gltf_gbuffer_ps;

        Microsoft::WRL::ComPtr<ID3D11PixelShader> gltf_ps;

        // SHADOW
        const uint32_t shadowmap_width = 2048;
        const uint32_t shadowmap_height = 2048;
        std::unique_ptr<ShadowMap> double_speed_z;
        DirectX::XMFLOAT4 light_view_focus{ 0, 0, 0, 1 };
        float light_view_distance{ 10.0f };
        float light_view_size{ 12.0f };
        float light_view_near_z{ 2.0f };
        float light_view_far_z{ 18.0f };


        bool enable_shadow = true;
        bool enable_skymap = false;
        bool enable_bloom = false;
        bool enable_fog = false;

        bool enable_deferred = false;
        bool enable_deferred_post = false;
        bool enable_post_effect = false;
    private:
        bool ready = false;
    };

}