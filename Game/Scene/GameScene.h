#pragma once
#include "./Lemur/Scene/BaseScene.h"

//Game
#include "../Stage/Stage.h"
#include "../Stage/Fence.h"
#include "../Character/Player.h"
#include "../Stage/StageMain.h"

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

    // Imgui
    void DebugImgui()override;

    // �����̏�����
    void InitializeLight()override;

    void PauseUpdate(float elapsedTime);

    // �X�^�[�g�O�̉��o
    void StartDirectionUpdate(float elapsedTime);
    void OverDirectionUpdate(float elapsedTime);
    void ClearDirectionUpdate(float elapsedTime);
private:// �Q�[��
    StageMain* stage_main;

    Stage* stage = nullptr;

    Player* player = nullptr;

    Fence* fence = nullptr;

    float   timer = 0.0f;// �^�C�}�[
    float   time_limit;// �^�C�����~�b�g
    bool    time_up = false;// �^�C���A�b�v

    bool    preparation_next = false;
    int     next_scene                         = 0;
    bool    is_pause                         = false;
    int     pause_num                            = 0;
    EasingFunction pause_text_continue_scale = {};
    EasingFunction pause_text_select_scale   = {};

    // �X�^�[�g���o
    EasingFunction rotation_camera_x = {};
    EasingFunction scale_start_direction = {};
    float start_timer = 0.0f;
    bool start_direction = true;
    int start_direction_state = 0;
    float start_in_time = 0.5f;
    float start_out_time = 0.5f;
    enum START_DIRECTION
    {
        Camera_direction,
        Three,
        Three_End,
        Two,
        Two_End,
        One,
        One_End,
        START,
        START_End
    };


    bool over_direction = false;
    float over_timer = 0.0f;
    int over_state = 0;;

    bool clear_direction = false;
    int clear_direction_state = 0;
    float clear_timer = 0.0f;
    EasingFunction clear_text_scale = {};

    float timer_angle = 0.0f;
    // �R���g���[���[�p
    std::shared_ptr<Sprite> timer_hands;
    std::shared_ptr<Sprite> timer_ui_base;
    std::shared_ptr<Sprite> button_ui_base;
    std::shared_ptr<Sprite> button_ui_chara;
    std::shared_ptr<Sprite> button_ui_circle;

    std::shared_ptr<Sprite> pause_main;
    std::shared_ptr<Sprite> pause_text_continue;
    std::shared_ptr<Sprite> pause_text_select;

    std::shared_ptr<Sprite> start_text_3;
    std::shared_ptr<Sprite> start_text_2;
    std::shared_ptr<Sprite> start_text_1;
    std::shared_ptr<Sprite> start_text_start;
    std::shared_ptr<Sprite> start_text_clear;

    std::shared_ptr<Sprite> fight_enemy;

    std::shared_ptr<Sprite> pose_exp;


    std::unique_ptr<GltfModelManager> gltf_test_model;
private:// �V�F�[�_�[
    Microsoft::WRL::ComPtr<ID3D11PixelShader> unit_ps;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> enemy_ps;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> scarecrow_ps;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> sprite_ui;

    // shader
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_views[8];

    // dissolve
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> noise;

    // skyMap
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> skymap;

    std::unique_ptr<StaticMeshBatch>  static_meshes[8];

    // �O�p�`
    Triangle Try_T;
    // �~
    DirectX::XMFLOAT2 P0;
    float r0;

    // �Ńo�O
    std::unique_ptr<FbxModelManager> test_model;
    std::unique_ptr<FbxModelManager> test_model_2;
    Effect* hitEffect;
    Effekseer::Handle handle;
    // �f�o�b�O
    Rect rect;
    DirectX::XMFLOAT2 c_p;
    float c_r;
    float angle;
    // �����A�j���[�V�����t���p�[�e�B�N���V�X�e��
    std::unique_ptr<ParticleSystem> particle_bomb;


};