#include "Player.h"
#include "Lemur/Input/Input.h"
#include "Game/CollisionManager.h"
#include "SeedManager.h"
#include "UnitManager.h"
static Player* instance = nullptr;

Player::Player()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();

    // モデルの初期化
    model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources_2\\Model\\nico.fbx");

    // 座標を減算
    position.z = sub_pos_z;

    // とりあえずアニメーション
    model->PlayAnimation(0, true);
}

Player::~Player()
{
}

// 更新処理
void Player::Update(float elapsedTime)
{
    // はじき処理
    Flick(elapsedTime);

    // 入力処理
    InputProcess();

    // 速力処理更新
    UpdateVelocity(elapsedTime);

    // 無的時間更新
    UpdateInvincibleTimer(elapsedTime);

    // モデルアニメーション更新
    model->UpdateAnimation(elapsedTime);

    // 行列更新処理
    UpdateTransform();

    // Imgui
    DrawDebugGUI();

}

// 描画処理
void Player::Render(float scale, ID3D11PixelShader** replaced_pixel_shader)
{
    // デバッグ表示
    model->DrawDebug("Player");
    // かかしモデル描画
    model->Render(scale, replaced_pixel_shader);
}

// Imgui
void Player::DrawDebugGUI()
{
    ImGui::Begin("Player");

    ImGui::SliderFloat("moveSpeed", &moveSpeed, 0.0f, 10.0f);
    ImGui::SliderFloat("f_d", &f_d, 0.0f, 10.0f);
    
    ImGui::End();
}

// はじき処理
void Player::Flick(float elapsedTime)
{
    GamePad& gamePad = Input::Instance().GetGamePad();

    // コントローラーの右スティックY成分
    s_l = gamePad.GetAxisRY() * -1.0f;

    // 右スティックが動かされたとき
    if (s_l > 0.1f)
    {
        // タイマーを動かす
        timer_s += elapsedTime;
        // 最大値を更新し続ける
        if (s_l_max <= s_l)s_l_max = s_l;
    }
    else
    {
        // タイマーが0以上＝右スティックが離された時
        if (timer_s > 0)
        {
            // はじき距離を算出
            f_d = (s_l_max) / timer_s;
            // 初期化
            s_l_max = 0;
            timer_s = 0;
        }
    }

    //TODO この条件は暴発しかねないので要修正
    if (f_d >= 0.1f)
    {
        Seed* seed = new Seed();
        // ユニットがいないなら即座に発芽
        if (UnitManager::Instance().GetUnitCount() == 0)seed->SetBorn(true);
        // 種の種類を登録
        seed->SetCategory(unit_category);

        // ユニットの奥に行かないように
        f_d = CollisionManager::Instance().CollisionUnitBackVsSeed({ position.x ,f_d + sub_pos_z/*はじきで出た座標から、ステージの半径を減算*/ }).y;

        // 座標を確定
        seed->SetPosition(position.x/*プレイヤーのX座標*/, 0, f_d);
        // リストに追加
        SeedManager::Instance().Register(seed);
        // はじき距離を初期化
        f_d = 0.0f;
    }

}

// 入力処理
void Player::InputProcess()
{
    GamePad& gamePad = Input::Instance().GetGamePad();

    // 左スティックX成分をスピードに変換
    velocity.x = gamePad.GetAxisLX() * moveSpeed;
}

Player& Player::Instance()
{
    return *instance;
}
