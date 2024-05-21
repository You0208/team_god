#pragma once
#include "./Lemur/Scene/BaseScene.h"
#include "./Lemur/Model/FbxModelManager.h"
#include "./Lemur/Model/GltfModelManager.h"
#include "./Lemur/Effekseer/Effect.h"


class FormationScene :public Lemur::Scene::BaseScene
{
public:
    FormationScene() {}
    ~FormationScene() override {}

    // ������
    void Initialize()override;

    // �I����
    void Finalize()override;

    // �X�V����
    void Update(HWND hwnd, float elapsedTime)override;

    // �`�揈��
    void Render(float elapsedTime)override;

    // ���j�b�g�̍X�V
    void UpdateUnit(float elapsedTime);

    // ����
    void UpdateOperate(float elapsedTime);

    // ImGui
    void DebugImgui()override;

    // ���C�g�̏�����
    void InitializeLight()override;

    void SelectUnit( int button_num);
private:
    // �{�^���̔ԍ�
    enum UNIT
    {
        Chili,
        Shishito,
        OrangePumpkin,
        GreenPumpkin,
        Broccoli,
        Cauliflower,
        J
    };
    static const int UNIT_MAX = 7;

    EasingFunction      button                  = {};       // �{�^���̃C�[�W���O
    EasingFunction      line_y                  = {};       // ���C���̃C�[�W���O
    EasingFunction      line_x                  = {};       // ���C���̃C�[�W���O
    EasingFunction      line_add                = {};     // ���j�b�g�V�p�̂��
    bool                enable_lineblue[4] = {};
    float               lineblue_pos_x[4];

    bool                once_only               = true;     

    int                 choose_num              = 0;        // ���ݑI�΂�Ă��郆�j�b�g�ԍ�
    int                 all_unit_num            = 0;        // �I�����ꂽ���j�b�g�̐�
    bool                enable_units[UNIT_MAX]         = {};       // ���j�b�g���g��ꂽ��
    float               units_rotation[UNIT_MAX]       = {};       // ���j�b�g�̊p�x
    DirectX::XMFLOAT3   units_position[UNIT_MAX]       = {};       // ���j�b�g�̈ʒu

    float               interval_timer          = 0.0f;     // �R���g���[������
    float               interval_timer_max      = 0.15f;    // �R���g���[���[�ő厞��


    bool                enable_controllers[4]   = {};       // �ǂ̃{�^�����I�΂�Ă��邩
    int                 cont_num[4]             = {};       // �{�^���ɂǂ̃��j�b�g���o�^����Ă��邩
    bool                select_button           = false;    // �{�^�����I������Ă��邩
    bool                first_touch             = true;     // �{�^���������̍ŏ��̈ꉟ����

    float               effect_scale            = 0.3f;     // �G�t�F�N�g�̃T�C�Y


    // �e�ꏊ�̈ʒu�Ɗp�x
    DirectX::XMFLOAT3 position[4] = {
        { 0.7f,-1.8f,0.0 },
        { -1.1f,-1.8f,0.0 },
        { -2.9f,-1.8f,0.0 },
        { -4.7f,-1.8f,0.0 }
    };
    float rotation[4] = {
        DirectX::XMConvertToRadians(-4.0f),
        DirectX::XMConvertToRadians(5.0f),
        DirectX::XMConvertToRadians(15.0f),
        DirectX::XMConvertToRadians(22.0f)
    };

    int status[UNIT_MAX][2] =
    {
        {2,4},
        {3,2},
        {4,1},
        {4,1},
        {1,1},
        {1,3},
        {2,2},
    };

private:
    // �e�N�X�`��
    std::shared_ptr<Sprite> back;
    std::shared_ptr<Sprite> front;
    std::shared_ptr<Sprite> line_1;
    std::shared_ptr<Sprite> line_2;
    std::shared_ptr<Sprite> line_blue;
    std::shared_ptr<Sprite> unit_line[UNIT_MAX];
    std::shared_ptr<Sprite> Button;
    std::shared_ptr<Sprite> base;
    std::shared_ptr<Sprite> Controller_UI[4];
    std::shared_ptr<Sprite> mark_1;
    std::shared_ptr<Sprite> mark_1_1;
    std::shared_ptr<Sprite> mark_2;
    std::shared_ptr<Sprite> mark_2_2;

    std::unique_ptr<GltfModelManager> gltf_unit[UNIT_MAX];

    // �f�o�b�O�p�̏��\��
    std::unique_ptr<GltfModelManager> debug_;
    std::unique_ptr<FbxModelManager> test_model_2;

    Effect* effect = nullptr;          // �U���G�t�F�N�g
    Effekseer::Handle effect_handle[4] = {};          // �U���G�t�F�N�g
};