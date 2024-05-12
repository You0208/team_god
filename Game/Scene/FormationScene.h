
#pragma once
#include "./Lemur/Scene/BaseScene.h"
#include "./Lemur/Model/FbxModelManager.h"
#include "./Lemur/Model/GltfModelManager.h"


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

    void DebugImgui()override;

    void InitializeLight()override;
private:
    int choose_num = 0;// ���ݑI�΂�Ă��郆�j�b�g�ԍ�
    int all_unit_num;// �I�����ꂽ���j�b�g�̐�
    int all_controllers_num;// ���ԖڂɑI�΂�Ă���̂�

    bool enable_units[6];
    DirectX::XMFLOAT3 units_position[6];
    float units_rotation[6];

    float interval_timer = 0.0f;// �R���g���[������
    float interval_timer_max = 0.15f;// �R���g���[���[�ő厞��

    bool enable_controllers[4];
    DirectX::XMFLOAT2 controllers_position[4];
    int controllers_num[4];// ���ԖڂɑI�΂ꂽ��
    int cont_num[4];

    EasingFunction button;

    std::shared_ptr<Sprite> back;
    std::shared_ptr<Sprite> line_1;
    std::shared_ptr<Sprite> line_2;
    std::shared_ptr<Sprite> unit_1;
    std::shared_ptr<Sprite> unit_2;
    std::shared_ptr<Sprite> unit_3;
    std::shared_ptr<Sprite> unit_4;
    std::shared_ptr<Sprite> unit_5;
    std::shared_ptr<Sprite> unit_6;
    std::shared_ptr<Sprite> Button;
    std::shared_ptr<Sprite> base;
    std::shared_ptr<Sprite> Controller_UI_A;
    std::shared_ptr<Sprite> Controller_UI_B;
    std::shared_ptr<Sprite> Controller_UI_X;
    std::shared_ptr<Sprite> Controller_UI_Y;
    std::shared_ptr<Sprite> mark_1;
    std::shared_ptr<Sprite> mark_1_1;
    std::shared_ptr<Sprite> mark_2;
    std::shared_ptr<Sprite> mark_2_2;

    std::unique_ptr<GltfModelManager> gltf_unit_1;
    std::unique_ptr<GltfModelManager> gltf_unit_2;
    std::unique_ptr<GltfModelManager> gltf_unit_3;
    std::unique_ptr<GltfModelManager> gltf_unit_4;
    std::unique_ptr<GltfModelManager> gltf_unit_5;
    std::unique_ptr<GltfModelManager> gltf_unit_6;

    // �f�o�b�O�p�̏��\��
    std::unique_ptr<GltfModelManager> debug_;
    std::unique_ptr<FbxModelManager> test_model_2;

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

    bool select_button = false;
};