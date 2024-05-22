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

    // 初期化
    void Initialize()override;

    // 終了化
    void Finalize()override;

    // 更新処理
    void Update(HWND hwnd, float elapsedTime)override;

    // 描画処理
    void Render(float elapsedTime)override;

    // Imgui
    void DebugImgui()override;

    // 光源の初期化
    void InitializeLight()override;

    void PauseUpdate(float elapsedTime);
private:// ゲーム
    bool    is_pause = false;
    int     pause_num = 0;
    EasingFunction pause_text_continue_scale = {};
    EasingFunction pause_text_select_scale = {};

    float timer_angle = 0.0f;
    // コントローラー用
    std::shared_ptr<Sprite> button_ui_base;
    std::shared_ptr<Sprite> button_ui_chara;

    std::shared_ptr<Sprite> pause_main;
    std::shared_ptr<Sprite> pause_text_continue;
    std::shared_ptr<Sprite> pause_text_select;

    std::shared_ptr<Sprite> tutorial_gif[8];

    std::shared_ptr<Sprite> button[4] = {};
    std::shared_ptr<Sprite> mission_text[4] = {};
    std::shared_ptr<Sprite> menu_text = {};
    std::shared_ptr<Sprite> reset_text = {};
    std::shared_ptr<Sprite> button_beside = {};
    std::shared_ptr<Sprite> button_up = {};
    std::shared_ptr<Sprite> left_row = {};
    std::shared_ptr<Sprite> right_row = {};
    std::shared_ptr<Sprite> Frame_tutorial = {};
    std::shared_ptr<Sprite> left_text = {};
    std::shared_ptr<Sprite> right_text = {};


    int tutorial_glf_num_x[8] = {2,0,5,0,9,9,9,4};// 一番下のラインのとこ

    int tutorial_gif_num = 0;

    int tutorial_state = 0;
    int unit_category = 0;
    DirectX::XMFLOAT3 player_current_position = {};
    enum MISSION
    {
        throw_seed,
        move_player,
        change_unit,
        attack,
        clear
    };

    Fence* fence = nullptr;

    Player* player = nullptr;

private:// シェーダー
    Microsoft::WRL::ComPtr<ID3D11PixelShader> unit_ps;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> enemy_ps;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> scarecrow_ps;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> sprite_ui;
    // dissolve
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> noise;

    // skyMap
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> skymap;

};