#pragma once
#include "./Lemur/Scene/BaseScene.h"

class SelectScene :public Lemur::Scene::BaseScene
{
public:
    SelectScene() {}
    ~SelectScene() override {}

    // 初期化
    void Initialize()override;

    // 終了化
    void Finalize()override;

    // 更新処理
    void Update(HWND hwnd, float elapsedTime)override;

    // 描画処理
    void Render(float elapsedTime)override;

    void DebugImgui()override;

private:
    // 演出
    void Direction();
    // 操作
    void HandleInput(float elapsedTime);
private:
    // levelセット用
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

    bool            first_touch                 = true;// 選択のブロック
    float           touch_interval              = 0.0f;// 選択インターバル
    int             world_num                   = 0;    // ワールド番号
    int             stage_num                   = 0;    // ステージ番号
    bool            switch_direction            = false;//切り替えアニメーション実行フラグ
    bool            add                         = false;// プラスか
    bool            minus                       = false;// マイナスか
    int             direction_num               = false;// 演出番号

    // 演出用
    EasingFunction  transition_angle            = {};   // 黒背景
    // 入りの線
    EasingFunction  transition_line1_angle      = {};
    EasingFunction  transition_line2_angle      = {};
    EasingFunction  transition_line3_angle      = {};
    // 引きの線                                    
    EasingFunction  transition_line1_angle_2    = {};
    EasingFunction  transition_line2_angle_2    = {};
    EasingFunction  transition_line3_angle_2    = {};

private:
    // ステージの色味調整用
    Microsoft::WRL::ComPtr<ID3D11PixelShader> select_ps;

    // UI
    std::shared_ptr<Sprite> ui                  = nullptr;
    std::shared_ptr<Sprite> ui_1                  = nullptr;
    std::shared_ptr<Sprite> ui_2                  = nullptr;

    // 案山子
    std::shared_ptr<Sprite> kakashi_1           = nullptr;
    std::shared_ptr<Sprite> kakashi_2           = nullptr;
    std::shared_ptr<Sprite> kakashi_3           = nullptr;
    // ハイライト                                  
    std::shared_ptr<Sprite> highlight_1         = nullptr;
    std::shared_ptr<Sprite> highlight_2         = nullptr;
    std::shared_ptr<Sprite> highlight_3         = nullptr;
    // ステージ                                    
    std::shared_ptr<Sprite> select_back         = nullptr;
    // じょうろ
    std::shared_ptr<Sprite> joro                = nullptr;
    // 遷移
    std::shared_ptr<Sprite> transition          = nullptr;
    std::shared_ptr<Sprite> transition_line1    = nullptr;
    std::shared_ptr<Sprite> transition_line2    = nullptr;
    std::shared_ptr<Sprite> transition_line3    = nullptr;
    std::shared_ptr<Sprite> transition_line1_2  = nullptr;
    std::shared_ptr<Sprite> transition_line2_2  = nullptr;
    std::shared_ptr<Sprite> transition_line3_2  = nullptr;

    std::shared_ptr<Sprite> window;
    std::shared_ptr<Sprite> window_title;
    std::shared_ptr<Sprite> window_yes;
    std::shared_ptr<Sprite> window_no;
    EasingFunction  pause_window_scale = {};     // ポーズセレクト画面イージング
    EasingFunction          yes = {};                   // セレクトサイズ用
    EasingFunction          no = {};                   // もう一度のサイズ用
    enum Button
    {
        YES,
        NO
    };
    int  select_num = 1;                    // 今何のボタンが選ばれているか
    bool is_title_window = false;  // ポーズフラグ
    bool is_title_next = false;  // ポーズフラグ
};
