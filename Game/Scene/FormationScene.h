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

    // 初期化
    void Initialize()override;

    // 終了化
    void Finalize()override;

    // 更新処理
    void Update(HWND hwnd, float elapsedTime)override;

    // 描画処理
    void Render(float elapsedTime)override;

    // ユニットの更新
    void UpdateUnit(float elapsedTime);

    // 操作
    void HandleInput(float elapsedTime);

    // ImGui
    void DebugImgui()override;

    // ライトの初期化
    void InitializeLight()override;

    void SelectUnit( int button_num);
private:
    // ボタンの番号
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

    EasingFunction      button                         = {};       // ボタンのイージング
    EasingFunction      line_y                         = {};       // ラインのイージング
    EasingFunction      line_x                         = {};       // ラインのイージング
    EasingFunction      line_add                       = {};       // ユニット７用のやつ
    bool                enable_lineblue[4]             = {};
    float               lineblue_pos_x[4];

    bool                once_only                      = true;     

    int                 choose_num                     = 0;        // 現在選ばれているユニット番号
    int                 all_unit_num                   = 0;        // 選択されたユニットの数
    bool                enable_units[UNIT_MAX]         = {};       // ユニットが使われたか
    float               units_rotation[UNIT_MAX]       = {};       // ユニットの角度
    DirectX::XMFLOAT3   units_position[UNIT_MAX]       = {};       // ユニットの位置
    EasingFunction      unit_size[UNIT_MAX]            = {};       // 選択中のユニットのサイズ

    float               interval_timer                 = 0.0f;     // コントローラ時間
    float               interval_timer_max             = 0.15f;    // コントローラー最大時間

    bool                enable_controllers[4]          = {};       // どのボタンが選ばれているか
    int                 cont_num[4]                    = {};       // ボタンにどのユニットが登録されているか
    bool                select_button                  = false;    // ボタンが選択されているか
    bool                first_touch                    = true;     // ボタン長押しの最初の一押しか

    float               effect_scale                   = 0.3f;     // エフェクトのサイズ
    int                 alpha                          = 1;
    float               blue_y[4]                      = {};
    bool                is_next_select                 = false;
    bool                is_next_game                   = false;

    // 各場所の位置と角度
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
        {1,3},
        {1,3},
        {2,2},
    };

private:
    // ステージの色味調整用
    Microsoft::WRL::ComPtr<ID3D11PixelShader> select_ps;

    // テクスチャ
    std::shared_ptr<Sprite> back;
    std::shared_ptr<Sprite> front;
    std::shared_ptr<Sprite> line_1;
    std::shared_ptr<Sprite> line_2;
    std::shared_ptr<Sprite> line_4;
    std::shared_ptr<Sprite> line_blue;
    std::shared_ptr<Sprite> unit_line[UNIT_MAX];
    std::shared_ptr<Sprite> Button;
    std::shared_ptr<Sprite> base;
    std::shared_ptr<Sprite> Controller_UI[4];
    std::shared_ptr<Sprite> mark_1;
    std::shared_ptr<Sprite> mark_1_1;
    std::shared_ptr<Sprite> mark_2;
    std::shared_ptr<Sprite> mark_2_2;
    std::shared_ptr<Sprite> Best;
    std::shared_ptr<Sprite> arrow;

    std::shared_ptr<Sprite> unit_attack[UNIT_MAX];

    bool enable_best[9][UNIT_MAX] =
    {
        {true,false,false,false,false,false,false},
        {true,false,false,false,false,false,false},
        {true,false,false,false,false,false,false},
        {true,false,false,false,true,false,false},
        {true,false,false,false,false,false,false},
        {true,false,false,false,false,true,false},
        {true,false,true,true,false,true,false},
        {false,false,true,true,false,false,false},
        {false,false,true,true,false,false,false},
    };

    std::unique_ptr<GltfModelManager> gltf_unit[UNIT_MAX];

    // デバッグ用の床表示
    std::unique_ptr<GltfModelManager> debug_;
    std::unique_ptr<FbxModelManager> test_model_2;

    Effect* effect = nullptr;          // 攻撃エフェクト
    Effekseer::Handle effect_handle[4] = {};          // 攻撃エフェクト
};