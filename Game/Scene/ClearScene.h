#pragma once
#include "./Lemur/Scene/BaseScene.h"

class ClearScene :public Lemur::Scene::BaseScene
{
public:
    ClearScene() {}
    ~ClearScene() override {}

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
    void Direction(float elapsedTime);
    void HandleInput(float elapsedTime);
private:
    enum Button
    {
        Next,
        Select,
        Again
    };

    // �C�[�W���O
    EasingFunction star1_scale = {};
    EasingFunction star2_scale = {};
    EasingFunction star3_scale = {};
    EasingFunction select      = {};
    EasingFunction again       = {};
    EasingFunction next        = {};

    // ��
    bool is_star_1       = true;
    bool is_star_2       = true;
    bool is_star_3       = true;

    bool once_only       = true;  // ���o�p

    bool is_continue     =false;

    // �ǂ̉��o��
    int  direction_num_1 = 0; 
    int  direction_num_2 = 0; 
    int  direction_num_3 = 0; 

    // �^�C�}�[�֘A
    float timer          = 0.0f;
    float interval_1     = 0.5f;
    float interval_2     = 1.5f;
    float interval_3     = 2.0f;

    // �I��
    int select_num       = 0;
    bool first_touch     = true;
    float touch_interval = 0.0f;
private:
    std::shared_ptr<Sprite> clear_back       = nullptr; // �N���A��{�̔w�i
    std::shared_ptr<Sprite> clear_start_back = nullptr; // �I�����ꂽ���̔w�i

    std::shared_ptr<Sprite> result_cover     = nullptr; // ������x
    std::shared_ptr<Sprite> star1            = nullptr; // ��
    std::shared_ptr<Sprite> star2            = nullptr; // ��
    std::shared_ptr<Sprite> star3            = nullptr; // ��
    std::shared_ptr<Sprite> star_frame1      = nullptr; // ���g
    std::shared_ptr<Sprite> star_frame2      = nullptr; // ���g
    std::shared_ptr<Sprite> star_frame3      = nullptr; // ���g
    std::shared_ptr<Sprite> next_text        = nullptr; // ����
    std::shared_ptr<Sprite> select_text      = nullptr; // �Z���N�g��ʂ�
    std::shared_ptr<Sprite> again_text       = nullptr; // ������x
};