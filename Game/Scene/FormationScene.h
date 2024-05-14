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
    void UpdateOperate(float elapsedTime);

    // ImGui
    void DebugImgui()override;

    // ライトの初期化
    void InitializeLight()override;

    void SelectUnit(bool BYN, int button_num);
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

    EasingFunction      button                  = {};       // ボタンのイージング
    EasingFunction      line_y                  = {};       // ラインのイージング

    bool                once_only               = true;     

    int                 choose_num              = 0;        // 現在選ばれているユニット番号
    int                 all_unit_num            = 0;        // 選択されたユニットの数
    bool                enable_units[6]         = {};       // ユニットが使われたか
    float               units_rotation[6]       = {};       // ユニットの角度
    DirectX::XMFLOAT3   units_position[6]       = {};       // ユニットの位置

    float               interval_timer          = 0.0f;     // コントローラ時間
    float               interval_timer_max      = 0.15f;    // コントローラー最大時間


    bool                enable_controllers[4]   = {};       // どのボタンが選ばれているか
    int                 cont_num[4]             = {};       // ボタンにどのユニットが登録されているか
    bool                select_button           = false;    // ボタンが選択されているか

    float               effect_scale            = 0.3f;     // エフェクトのサイズ

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

    int status[6][2] =
    {
        {1,1},
        {1,1},
        {1,1},
        {1,1},
        {1,1},
        {1,1}
    };

private:
    // テクスチャ
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
    std::shared_ptr<Sprite> Controller_UI[4];
    std::shared_ptr<Sprite> mark_1;
    std::shared_ptr<Sprite> mark_1_1;
    std::shared_ptr<Sprite> mark_2;
    std::shared_ptr<Sprite> mark_2_2;

    std::unique_ptr<GltfModelManager> gltf_unit[6];

    // デバッグ用の床表示
    std::unique_ptr<GltfModelManager> debug_;
    std::unique_ptr<FbxModelManager> test_model_2;

    Effect* effect = nullptr;          // 攻撃エフェクト
};