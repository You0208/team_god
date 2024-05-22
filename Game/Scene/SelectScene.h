#pragma once
#include "./Lemur/Scene/BaseScene.h"

class SelectScene :public Lemur::Scene::BaseScene
{
public:
    SelectScene() {}
    ~SelectScene() override {}

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
    // level�Z�b�g�p
    int set_level[3][3] = {
        {0,1,2},
        {3,4,5},
        {6,7,8}
    };
   
    DirectX::XMFLOAT2 stage_mask_pos[3] = {
        { 360, 570},
        {960,570},
        { 1560,570 }
    };

    bool first_touch = true;
    float touch_interval = 0.0f;
    int             world_num                   = 0;    // ���[���h�ԍ�
    int             stage_num                   = 0;    // �X�e�[�W�ԍ�
    bool            switch_direction            = false;//�؂�ւ��A�j���[�V�������s�t���O
    bool            add                         = false;// �v���X��
    bool            minus                       = false;// �}�C�i�X��
    int             direction_num               = false;// ���o�ԍ�

    // ���o�p
    EasingFunction  transition_angle            = {};   // ���w�i
    // ����̐�
    EasingFunction  transition_line1_angle      = {};
    EasingFunction  transition_line2_angle      = {};
    EasingFunction  transition_line3_angle      = {};
    // �����̐�                                    
    EasingFunction  transition_line1_angle_2    = {};
    EasingFunction  transition_line2_angle_2    = {};
    EasingFunction  transition_line3_angle_2    = {};

private:
    // �X�e�[�W�̐F�������p
    Microsoft::WRL::ComPtr<ID3D11PixelShader> select_ps;

    // UI
    std::shared_ptr<Sprite> ui                  = nullptr;
    std::shared_ptr<Sprite> ui_1                  = nullptr;
    std::shared_ptr<Sprite> ui_2                  = nullptr;

    // �ĎR�q
    std::shared_ptr<Sprite> kakashi_1           = nullptr;
    std::shared_ptr<Sprite> kakashi_2           = nullptr;
    std::shared_ptr<Sprite> kakashi_3           = nullptr;
    // �n�C���C�g                                  
    std::shared_ptr<Sprite> highlight_1         = nullptr;
    std::shared_ptr<Sprite> highlight_2         = nullptr;
    std::shared_ptr<Sprite> highlight_3         = nullptr;
    // �X�e�[�W                                    
    std::shared_ptr<Sprite> select_back         = nullptr;
    // ���傤��
    std::shared_ptr<Sprite> joro                = nullptr;
    // �J��
    std::shared_ptr<Sprite> transition          = nullptr;
    std::shared_ptr<Sprite> transition_line1    = nullptr;
    std::shared_ptr<Sprite> transition_line2    = nullptr;
    std::shared_ptr<Sprite> transition_line3    = nullptr;
    std::shared_ptr<Sprite> transition_line1_2  = nullptr;
    std::shared_ptr<Sprite> transition_line2_2  = nullptr;
    std::shared_ptr<Sprite> transition_line3_2  = nullptr;
};
