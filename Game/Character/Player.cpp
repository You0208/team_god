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

    // 左右端初期化
    limit = { StageManager::Instance().GetStage(StageManager::Instance().GetStageIndex())->GetStageCollision().left_up.x + 0.5f,
         StageManager::Instance().GetStage(StageManager::Instance().GetStageIndex())->GetStageCollision().right_down.x - 0.5f };

    // ユニットカテゴリーの初期化
    unit_category = UnitManager::UNIT_INDEX::GreenPumpkin;

    //TODO もね　ゲーム画面
    {
        // はじく強さ
        flip_speed = 2.0f;
        // 種の射出速度
        seed_throw_speed = 21.0f;
        // 案山子が柵から離れる距離
        sub_pos_z_puls = 0.55f;
        // 案山子の移動速度
        moveSpeed = 5.1f;
        // 案山子から種の最短距離
        dis_scarecrow = 1.0f;
    }
    // 案山子の初期位置修正
    sub_pos_z = StageManager::Instance().GetStage(StageManager::Instance().GetStageIndex())->GetVariableStageWidth().y + sub_pos_z_puls;
    position.z = -sub_pos_z;
    // とりあえずアニメーション
    PlayAnimation(Animation::Idle, true);
}

Player::~Player()
{
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

        // Imgui
        DrawDebugGUI();
    }

    // 入力処理
    InputProcess();

    // 経過フレーム
    float elapsed_frame = 60.0f * elapsedTime;
   // FlickRe(elapsed_frame);
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
    ImGui::Begin(u8"案山子とか");

    ImGui::SliderFloat(u8"移動速度", &moveSpeed, 0.0f, 10.0f);
    ImGui::SliderFloat(u8"ステージとの距離", &sub_pos_z_puls, 0.0f, 5.0f);
    ImGui::SliderFloat(u8"種の射出速度", &seed_throw_speed, 0.0f, 100.0f);
    ImGui::SliderFloat(u8"はじく強さここで変えれます", &flip_speed, 0.1f, 10.0f);
    ImGui::SliderFloat(u8"コントローラー無い時のはじき", &flip_pos_z, 0.0f, 50.0f);
    ImGui::SliderFloat(u8"溜めの最大時間", &max_charge_time, 0.0f, 3.0f);
    ImGui::SliderFloat(u8"種が落ちる一番手前", &dis_scarecrow, 0.0f, 3.0f);
    
    ImGui::End();
}

// はじき処理
void Player::Flick(float elapsedTime)
{
    GamePad& gamePad = Input::Instance().GetGamePad();

    // コントローラーの右スティックY成分
    right_stick_y = gamePad.GetAxisRY() * -1.0f;

    // 右スティックが動かされたとき（引っ張られた時）
    if ( right_stick_y > 0.1f)
    {
        // 引っ張りモーションへ
        // 横移動できないように
        velocity.x = 0;
        PlayAnimation(Animation::Pull, false);

        if (flip_timer > 0.5f)
        {
            //// 投げアニメーションへ
            PlayAnimation(Animation::Throw, false);
            //// 横移動できないように
            //velocity.x = 0;

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
                flip_pos_z = sub_pos_z_puls-dis_scarecrow;
            }

            // スケーリング
            float scaling = StageManager::Instance().GetStage(StageManager::Instance().GetStageIndex())->GetVariableStageWidth().y * 2- dis_scarecrow;
            //flip_pos_z = scaling * (flip_pos_z / scaling);
            // 最小値0、最大値scalingにクランプする
            flip_pos_z = std::clamp(flip_pos_z, 0.0f, scaling);
            flip_pos_z = (scaling + sub_pos_z_puls + dis_scarecrow) - flip_pos_z;

            max_right_stick_y = 0;
            right_stick_y = 0;
            flip_timer = 0;

            is_throw = true;
            //flip_pos_z = (scaling+sub_pos_zt) - flip_pos_z;
            //flip_pos_z = flip_pos_z * -1.0f;
        }
    }

    //TODO この条件は暴発しかねないので要修正
    if (is_throw)
    {
        //
        is_throw = false;
        // 横移動出来ないように
        velocity.x = 0;
        // 種の実体を生成
        Seed* seed = new Seed();
        // 一番近いユニットの場所を渡す
        DirectX::XMFLOAT2 unit_pos;

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

    // TODO もね　ユニットのボタン設定
    Lemur::Scene::SceneManager::Instance().set_unit_cont[gamePad.B]= UnitManager::UNIT_INDEX::Chili;
    Lemur::Scene::SceneManager::Instance().set_unit_cont[gamePad.B]= UnitManager::UNIT_INDEX::Cauliflower;
    Lemur::Scene::SceneManager::Instance().set_unit_cont[gamePad.B]= UnitManager::UNIT_INDEX::Shishito;
    Lemur::Scene::SceneManager::Instance().set_unit_cont[gamePad.B]= UnitManager::UNIT_INDEX::GreenPumpkin;


    if (gamePad.GetButtonDown() & gamePad.BTN_B)unit_category = Lemur::Scene::SceneManager::Instance().set_unit_cont[gamePad.B];
    else if (gamePad.GetButtonDown() & gamePad.BTN_A)unit_category = Lemur::Scene::SceneManager::Instance().set_unit_cont[gamePad.A];
    else if (gamePad.GetButtonDown() & gamePad.BTN_X)unit_category =  Lemur::Scene::SceneManager::Instance().set_unit_cont[gamePad.X];
    else if (gamePad.GetButtonDown() & gamePad.BTN_Y)unit_category =  Lemur::Scene::SceneManager::Instance().set_unit_cont[gamePad.Y];
}

// 入力処理
void Player::InputProcess()
{
    GamePad& gamePad = Input::Instance().GetGamePad();
    if ((GetCurrentAnimationIndex() == Animation::Throw|| GetCurrentAnimationIndex() == Animation::Pull) && IsPlayAnimation())
    {
        velocity.x = 0;
    }
    else
    {
        // 左スティックX成分をスピードに変換
        velocity.x = gamePad.GetAxisLX() * moveSpeed;
        if (velocity.x < 0)  PlayAnimation(Animation::Left, false);
        else if (velocity.x > 0)  PlayAnimation(Animation::Right, false);
        else if (velocity.x == 0)  PlayAnimation(Animation::Idle, false);
    }
}

Player& Player::Instance()
{
    return *instance;
}
