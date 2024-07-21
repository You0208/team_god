#pragma once
#include "./Lemur/Scene/BaseScene.h"

//Game
#include "../Stage/Stage.h"
#include "../Stage/Fence.h"
#include "../Stage/StageMain.h"
#include "../Character/Player.h"

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
private:
    void PauseUpdate(float elapsedTime);
    // �X�^�[�g�O�̉��o
    void StartDirectionUpdate(float elapsedTime);
    void OverDirectionUpdate(float elapsedTime);
    void ClearDirectionUpdate(float elapsedTime);
private:// �Q�[��
    StageMain*      stage_main                = nullptr;
    Stage*          stage                     = nullptr;
    Player*         player                    = nullptr;
    Fence*          fence                     = nullptr;

    float           timer                     = 0.0f;   // �^�C�}�[
    float           time_limit                = 0.0f;   // �^�C�����~�b�g
    bool            time_up                   = false;  // �^�C���A�b�v

    bool            preparation_next          = false;  // �J�ډ��o�u���b�N�t���O
    int             next_scene                = 0;      // ���̃V�[��
    bool            is_pause                  = false;  // �|�[�Y�t���O
    int             pause_num                 = 0;      // �|�[�Y�őI������Ă���ԍ�
    EasingFunction  pause_text_continue_scale = {};     // �|�[�Y������x�C�[�W���O
    EasingFunction  pause_text_select_scale   = {};     // �|�[�Y�Z���N�g��ʃC�[�W���O

    // �X�^�[�g���o
    enum START_DIRECTION// �X�e�[�g
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
    EasingFunction  rotation_camera_x     = {};     // �J�����̃C�[�W���O
    EasingFunction  scale_start_direction = {};     // �傫���ύX�C�[�W���O
    float           start_timer           = 0.0f;   
    bool            start_direction       = true;   // ���o�t���O
    int             start_direction_state = 0;      // ���o�X�e�[�g
    float           start_in_time         = 0.5f;   // ����̍ő厞��
    float           start_out_time        = 0.5f;   // �Ō�̍ő厞��

    // �Q�[���I�[�o�[���o
    bool            over_direction        = false;
    float           over_timer            = 0.0f;
    int             over_state            = 0;;

    // �N���A���o
    bool            clear_direction       = false;
    int             clear_direction_state = 0;
    float           clear_timer           = 0.0f;
    EasingFunction  clear_text_scale      = {};

    // �^�C�}�[�̐j�p�x
    float           timer_angle           = 0.0f;

private:
    // �X�v���C�g
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

    // �V�F�[�_�[
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

    // �e�p���f��
    std::unique_ptr<FbxModelManager> shadow_model;

};