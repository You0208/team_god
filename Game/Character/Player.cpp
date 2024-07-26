#include "Player.h"
#include "Lemur/Input/Input.h"
#include "Game/CollisionManager.h"
#include "SeedManager.h"
#include "UnitManager.h"
#include "Game/Stage/StageManager.h"
#include "Game/Character/UnitManager.h"
#include "./Lemur/Scene/SceneManager.h"

static Player* instance = nullptr;

Player::Player()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();

    // モデルの初期化
    LoadFBXModel(graphics.GetDevice(), ".\\resources\\Model\\scarecrow_Re.fbx");

    gauge_base = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\UI\\gauge_base.png");
    gauge_frame = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\UI\\gauge_frame.png");
    gauge_bar = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\UI\\gauge_bar.png");

    // 左右端初期化
    limit = { StageManager::Instance().GetStage(StageManager::Instance().GetStageIndex())->GetStageCollision().left_up.x + 0.5f,
         StageManager::Instance().GetStage(StageManager::Instance().GetStageIndex())->GetStageCollision().right_down.x - 0.5f };

    GamePad& gamePad = Input::Instance().GetGamePad();

    // ユニットカテゴリーの初期化
    unit_category = Lemur::Scene::SceneManager::Instance().set_unit_cont[gamePad.A];

    // TODO もね　
    {
        // はじく強さ
        flip_speed = 2.0f;
        // 種の射出速度
        seed_throw_speed = 21.0f;
        // 案山子が柵から離れる距離
        sub_pos_z_puls = 1.5f;
        // 案山子の移動速度
        moveSpeed = 5.1f;
        // 案山子から種の最短距離
        dis_scarecrow = 0.8f;
    }
    // 案山子の初期位置修正
    sub_pos_z = StageManager::Instance().GetStage(StageManager::Instance().GetStageIndex())->GetVariableStageWidth().y + sub_pos_z_puls;
    position.z = -sub_pos_z;
    // とりあえずアニメーション
    PlayAnimation(Animation::Idle, true);

    // 姿勢を変えておく
    // 案山子位置の更新
    sub_pos_z = StageManager::Instance().GetStage(StageManager::Instance().GetStageIndex())->GetVariableStageWidth().y + sub_pos_z_puls;
    position.z = -sub_pos_z;

    // 行列更新処理
    UpdateTransform();
}

Player::~Player()
{
}

void Player::GaugeRender()
{
    float scaling_max = StageManager::Instance().GetStage(StageManager::Instance().GetStageIndex())->GetVariableStageWidth().y * 2;
    float scale = (mouse_timer - dis_scarecrow) / scaling_max;
    if (scaling_max <= mouse_timer)scale = 1.0f;

    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    ID3D11DeviceContext* immediate_context = graphics.GetDeviceContext();
    gauge_base->RenderCenter(immediate_context, position.x * 66 + 960 - 90, 960, 44, 206);
    gauge_bar->RenderLeftDown(immediate_context, position.x * 66 + 960 - 90-22, 960+103, 44, 206 * scale, 0);
    gauge_frame->RenderCenter(immediate_context, position.x * 66 + 960 - 90, 960, 44, 206);
}

void Player::GaugeRenderTutorial()
{
    float scaling_max = StageManager::Instance().GetStage(StageManager::Instance().GetStageIndex())->GetVariableStageWidth().y * 2;
    float scale = (mouse_timer - dis_scarecrow) / scaling_max;
    if (scaling_max <= mouse_timer)scale = 1.0f;

    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    ID3D11DeviceContext* immediate_context = graphics.GetDeviceContext();
    gauge_base->RenderCenter(immediate_context, position.x * 66 + 1430 - 90, 960, 44, 206);
    gauge_bar->RenderLeftDown(immediate_context, position.x * 66 + 1430 - 90 - 22, 960 + 103, 44, 206 * scale, 0);
    gauge_frame->RenderCenter(immediate_context, position.x * 66 + 1430 - 90, 960, 44, 206);
}

// 更新処理
void Player::Update(float elapsedTime)
{
    // 常に実行
    {
        // 速力処理更新
        UpdateVelocity(elapsedTime);

        // モデルアニメーション更新
        UpdateAnimation(elapsedTime);

        // 行列更新処理
        UpdateTransform();

        DrawDebugGUI();
    }

    // 入力処理
    InputProcess();

    // 経過フレーム
    float elapsed_frame = 60.0f * elapsedTime;
    // はじき処理
    Flick(elapsedTime);

    // カテゴリーの変更
    ChangeCategory();

    // 案山子位置の更新
    sub_pos_z = StageManager::Instance().GetStage(StageManager::Instance().GetStageIndex())->GetVariableStageWidth().y + sub_pos_z_puls;
    position.z = -sub_pos_z;

    // 左右端
    limit = { StageManager::Instance().GetStage(StageManager::Instance().GetStageIndex())->GetStageCollision().left_up.x + 0.5f,
         StageManager::Instance().GetStage(StageManager::Instance().GetStageIndex())->GetStageCollision().right_down.x - 0.5f };

    // 移動の制限
    position.x = (position.x <= limit.x) ? limit.x : (position.x >= limit.y) ? limit.y : position.x;
}

// Imgui
void Player::DrawDebugGUI()
{
    Mouse& mouse = Input::Instance().GetMouse();

#ifdef DEBUG_IMGUI
    ImGui::Begin(u8"案山子とか");

    float m = static_cast<float>(mouse.GetWheel())*0.001f;
    ImGui::DragFloat(u8"ホイール", &m, 0.0f);

    ImGui::SliderFloat(u8"移動速度", &moveSpeed, 0.0f, 10.0f);
    ImGui::SliderFloat(u8"ステージとの距離", &sub_pos_z_puls, 0.0f, 5.0f);
    ImGui::SliderFloat(u8"種の射出速度", &seed_throw_speed, 0.0f, 100.0f);
    ImGui::SliderFloat(u8"はじく強さここで変えれます", &flip_speed, 0.1f, 10.0f);
    ImGui::SliderFloat(u8"コントローラー無い時のはじき", &flip_pos_z, 0.0f, 50.0f);
    ImGui::SliderFloat(u8"溜めの最大時間", &max_charge_time, 0.0f, 3.0f);
    ImGui::SliderFloat(u8"種が落ちる一番手前", &dis_scarecrow, 0.0f, 3.0f);
    
    ImGui::End();
#endif
}

// はじき処理
void Player::Flick(float elapsedTime)
{
    GamePad& gamePad = Input::Instance().GetGamePad();
    Mouse& mouse = Input::Instance().GetMouse();

    // マウス用
    {
        float scaling_max = StageManager::Instance().GetStage(StageManager::Instance().GetStageIndex())->GetVariableStageWidth().y * 2;

        if (!is_mouse_click)
        {
            if (mouse.GetButtonDown() & mouse.BTN_LEFT)
            {
                is_mouse_click = true;
                mouse_timer = dis_scarecrow;
                PlayAnimation(Animation::Pull, false);
            }
            else
            {
                mouse_timer = dis_scarecrow;
            }
        }
        else
        {

            velocity.x = 0;
            if (scaling_max > mouse_timer)mouse_timer += elapsedTime * mouse_timer_speed;
            if (mouse.GetButtonUp() & mouse.BTN_LEFT)
            {
                PlayAnimation(Animation::Throw, false);
                // 最小値0、最大値scalingにクランプする
                flip_pos_z = mouse_timer;

                is_mouse_click = false;
                mouse_timer = dis_scarecrow;
                max_right_stick_y = 0;
                right_stick_y = 0;
                flip_timer = 0;

                is_throw = true;
            }
        }
    }

    // コントローラー用
    {
        // コントローラーの右スティックY成分
        right_stick_y = gamePad.GetAxisRY() * -1.0f;

        // 右スティックが動かされたとき（引っ張られた時）
        if (right_stick_y > 0.1f)
        {
            // 引っ張りモーションへ
            // 横移動できないように
            velocity.x = 0;
            PlayAnimation(Animation::Pull, false);

            if (flip_timer > 0.5f)
            {
                //// 投げアニメーションへ
                PlayAnimation(Animation::Throw, false);
                // タイマーを動かす
                flip_timer += elapsedTime;
                // 最大値を更新し続ける
                if (max_right_stick_y <= right_stick_y)max_right_stick_y = right_stick_y;
            }
            else
            {
                // タイマーを動かす
                flip_timer += elapsedTime;
                // 最大値を更新し続ける
                if (max_right_stick_y <= right_stick_y)max_right_stick_y = right_stick_y;
            }
        }
        else
        {
            // タイマーが0以上＝右スティックが離された時
            if (flip_timer > 0)
            {
                // 投げアニメーションへ
                PlayAnimation(Animation::Throw, false);
                // 横移動できないように
                velocity.x = 0;

                // はじき距離を算出
                flip_pos_z = (max_right_stick_y) / flip_timer * flip_speed;
                // 初期化
                if (flip_timer > max_charge_time)
                {
                    flip_pos_z = sub_pos_z_puls;
                }

                // スケーリング
                float scaling = StageManager::Instance().GetStage(StageManager::Instance().GetStageIndex())->GetVariableStageWidth().y * 2;
                // 最小値0、最大値scalingにクランプする
                flip_pos_z = std::clamp(flip_pos_z + dis_scarecrow, dis_scarecrow, scaling);
                flip_pos_z = (scaling + sub_pos_z_puls + dis_scarecrow) - flip_pos_z;

                // 初期化
                max_right_stick_y = 0;
                right_stick_y = 0;
                flip_timer = 0;

                is_throw = true;
            }
        }
    }

    // 投げられたら
    if (is_throw)
    {
        is_throw = false;
        // 横移動出来ないように
        velocity.x = 0;
        // 種の実体を生成
        Seed* seed = new Seed();
        // 一番近いユニットの場所を渡す
        DirectX::XMFLOAT2 unit_pos;

        // ユニットの後ろと種の判定
        if (!CollisionManager::CollisionUnitBackVsSeed_Re({ position.x ,flip_pos_z - sub_pos_z/*はじきで出た座標から、ステージの半径を減算*/ }, unit_pos))
        {
            // ユニットがいないなら即座に発芽
            if (UnitManager::Instance().GetUnitCount() == 0)    seed->SetBorn(true);
            // 座標を確定
            seed->SetPosition(position.x/*プレイヤーのX座標*/, 0, position.z);
            seed->SetDistinationPosition({ position.x/*プレイヤーのX座標*/, 0, flip_pos_z - sub_pos_z });
            // 種の種類を登録
            seed->SetCategory(unit_category);
            // 演出アリ
            seed->SetIsDirection(true);
            // 種の射出速度の設定
            seed->SetthrowSpeed(seed_throw_speed);
            // 姿勢更新
            seed->UpdateTransform();
            // リストに追加
            SeedManager::Instance().Register(seed);

            // はじき距離を初期化
            flip_pos_z = 0.0f;
        }
        else
        {
            // 座標を確定
            seed->SetPosition(position.x/*プレイヤーのX座標*/, 0, position.z);
            seed->SetDistinationPosition({ position.x/*プレイヤーのX座標*/, 0, unit_pos.y });
            // 種の射出速度の設定
            seed->SetthrowSpeed(seed_throw_speed);
            // 演出オン
            seed->SetIsDisDirection(true);
            // 姿勢更新
            seed->UpdateTransform();
            // リストに追加
            SeedManager::Instance().Register(seed);

            // はじき距離を初期化
            flip_pos_z = 0.0f;
        }
    }

}

// カテゴリーの変更
void Player::ChangeCategory()
{
    GamePad& gamePad = Input::Instance().GetGamePad();

    // TODO もね　
    //Chili (丸)
    //Shishito（バフ）
    //OrangePumpkin（縦三角）
    //GreenPumpkin（横三角）
    //Broccoli（縦）
    //Cauliflower（横）
    //ｊ（ドーナツ）

    //Lemur::Scene::SceneManager::Instance().set_unit_cont[gamePad.B]= UnitManager::UNIT_INDEX::J;
    //Lemur::Scene::SceneManager::Instance().set_unit_cont[gamePad.A]= UnitManager::UNIT_INDEX::Broccoli;
    //Lemur::Scene::SceneManager::Instance().set_unit_cont[gamePad.X]= UnitManager::UNIT_INDEX::GreenPumpkin;
    //Lemur::Scene::SceneManager::Instance().set_unit_cont[gamePad.Y]= UnitManager::UNIT_INDEX::OrangePumpkin;

    if (gamePad.GetButtonDown() & gamePad.BTN_B || GetAsyncKeyState('2') & 0x8000)unit_category = Lemur::Scene::SceneManager::Instance().set_unit_cont[gamePad.B];
    else if (gamePad.GetButtonDown() & gamePad.BTN_A || GetAsyncKeyState('3')  & 0x8000)unit_category = Lemur::Scene::SceneManager::Instance().set_unit_cont[gamePad.A];
    else if (gamePad.GetButtonDown() & gamePad.BTN_X || GetAsyncKeyState('4')  & 0x8000)unit_category = Lemur::Scene::SceneManager::Instance().set_unit_cont[gamePad.X];
    else if (gamePad.GetButtonDown() & gamePad.BTN_Y || GetAsyncKeyState('1')  & 0x8000)unit_category = Lemur::Scene::SceneManager::Instance().set_unit_cont[gamePad.Y];
}

// 入力処理
void Player::InputProcess()
{
    GamePad& gamePad = Input::Instance().GetGamePad();
    Mouse& mouse = Input::Instance().GetMouse();

    if ((GetCurrentAnimationIndex() == Animation::Throw|| GetCurrentAnimationIndex() == Animation::Pull) && IsPlayAnimation() || is_mouse_click)
    {
        velocity.x = 0;
    }
    else
    {
        // 左スティックX成分をスピードに変換
        velocity.x = gamePad.GetAxisLX() * moveSpeed;

        // キーマウ用
        if (GetAsyncKeyState('D'))velocity.x = moveSpeed;
        else if (GetAsyncKeyState('A'))velocity.x = -moveSpeed;

        // アニメーション
        if (velocity.x < 0)  PlayAnimation(Animation::Left, false);
        else if (velocity.x > 0)  PlayAnimation(Animation::Right, false);
        else if (velocity.x == 0)  PlayAnimation(Animation::Idle, false);
    }
}

Player& Player::Instance()
{
    return *instance;
}
