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
private:
    void PauseUpdate(float elapsedTime);
    // スタート前の演出
    void StartDirectionUpdate(float elapsedTime);
    void OverDirectionUpdate(float elapsedTime);
    void ClearDirectionUpdate(float elapsedTime);
private:// ゲーム
    StageMain*      stage_main                = nullptr;
    Stage*          stage                     = nullptr;
    Player*         player                    = nullptr;
    Fence*          fence                     = nullptr;

    float           timer                     = 0.0f;   // タイマー
    float           time_limit                = 0.0f;   // タイムリミット
    bool            time_up                   = false;  // タイムアップ

    bool            preparation_next          = false;  // 遷移演出ブロックフラグ
    int             next_scene                = 0;      // 次のシーン
    bool            is_pause                  = false;  // ポーズフラグ
    int             pause_num                 = 0;      // ポーズで選択されている番号
    EasingFunction  pause_text_continue_scale = {};     // ポーズもう一度イージング
    EasingFunction  pause_text_select_scale   = {};     // ポーズセレクト画面イージング

    // スタート演出
    enum START_DIRECTION// ステート
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
    EasingFunction  rotation_camera_x     = {};     // カメラのイージング
    EasingFunction  scale_start_direction = {};     // 大きさ変更イージング
    float           start_timer           = 0.0f;   
    bool            start_direction       = true;   // 演出フラグ
    int             start_direction_state = 0;      // 演出ステート
    float           start_in_time         = 0.5f;   // 入りの最大時間
    float           start_out_time        = 0.5f;   // 最後の最大時間

    // ゲームオーバー演出
    bool            over_direction        = false;
    float           over_timer            = 0.0f;
    int             over_state            = 0;;

    // クリア演出
    bool            clear_direction       = false;
    int             clear_direction_state = 0;
    float           clear_timer           = 0.0f;
    EasingFunction  clear_text_scale      = {};

    // タイマーの針角度
    float           timer_angle           = 0.0f;

private:
    // スプライト
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

    // シェーダー
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

    // 影用モデル
    std::unique_ptr<FbxModelManager> shadow_model;

};