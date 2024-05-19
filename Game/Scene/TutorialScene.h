#pragma once
#include "./Lemur/Scene/BaseScene.h"

//Game
#include "../Stage/Stage.h"
#include "../Stage/Fence.h"
#include "../Character/Player.h"

class TutorialScene :public Lemur::Scene::BaseScene
{
public:
    TutorialScene() {}
    ~TutorialScene() override {}

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
private:// �Q�[��
    bool    is_pause = false;
    int     pause_num = 0;
    EasingFunction pause_text_continue_scale = {};
    EasingFunction pause_text_select_scale = {};

    float timer_angle = 0.0f;
    // �R���g���[���[�p
    std::shared_ptr<Sprite> button_ui_base;
    std::shared_ptr<Sprite> button_ui_chara;

    std::shared_ptr<Sprite> pause_main;
    std::shared_ptr<Sprite> pause_text_continue;
    std::shared_ptr<Sprite> pause_text_select;

    std::shared_ptr<Sprite> tutorial_glf[6];

    int tutorial_num = 0;

    Fence* fence = nullptr;

    Player* player = nullptr;

private:// �V�F�[�_�[
    Microsoft::WRL::ComPtr<ID3D11PixelShader> unit_ps;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> enemy_ps;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> scarecrow_ps;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> sprite_ui;
    // dissolve
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> noise;

    // skyMap
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> skymap;

};