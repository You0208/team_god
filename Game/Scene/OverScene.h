#pragma once
#include "./Lemur/Scene/BaseScene.h"

class OverScene :public Lemur::Scene::BaseScene
{
public:
    OverScene() {}
    ~OverScene() override {}

    // ������
    void Initialize()override;

    // �I����
    void Finalize()override;

    // �X�V����
    void Update(HWND hwnd, float elapsedTime)override;

    // �`�揈��
    void Render(float elapsedTime)override;

    void DebugImgui()override;
private:
    enum Button
    {
        Select,
        Again
    };
    std::shared_ptr<Sprite> game_over_back      = nullptr;              //�Q�[���I�[�o�[��{�̔w�i
    std::shared_ptr<Sprite> continue_back       = nullptr;              // �I�����ꂽ���̔w�i
    std::shared_ptr<Sprite> ui_text[8]          = {};                   // �Q�[���I�[�o�[�̕���
    std::shared_ptr<Sprite> again_text          = nullptr;              // ������x
    std::shared_ptr<Sprite> stageselect_text    = nullptr;              // ���ɖ߂�

    EasingFunction          text_y[8]           = {};                   // ������Y
    EasingFunction          text_angle[8]       = {};                   // �����̊p�x

    int                     select_num          = 0;                    // �����̃{�^�����I�΂�Ă��邩

    EasingFunction          select              = {};                   // �Z���N�g�T�C�Y�p
    EasingFunction          again               = {};                   // ������x�̃T�C�Y�p

    bool                    once_only           = true;                 // ���o�p

    int                     direction_num       = 0;                    // �ǂ̉��o��
    int                     random_num[8]       = { 0,7,5,4,3,1,6,2 };  // �p�x����������i��ł��邩

    float                   interval            = 0.0f;                 // �C���^�[�o��
    float                   timer               = 0.0f;                 // �^�C�}�[

    bool                    anim_stop           = true;                 // �A�j�����X�g�b�v���ꂽ��
    bool                    is_continue         = false;                // �{�^�����I�΂ꂽ��
    int                     count_num           = 0;                    // �����J�E���g
};