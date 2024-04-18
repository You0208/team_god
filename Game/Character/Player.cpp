#include "Player.h"
#include "Lemur/Input/Input.h"
#include "Game/CollisionManager.h"
#include "SeedManager.h"
#include "UnitManager.h"
#include "Game/Stage/StageManager.h"

static Player* instance = nullptr;

Player::Player()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();

    // モデルの初期化
    model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\scarecrow_Re.fbx");

    sub_pos_z = -StageManager::Instance().GetStage(StageManager::Instance().GetStageIndex())->GetVariableStageWidth().y-1.0f;

    // 座標を減算
    position.z = sub_pos_z;

    // 左右端
    limit = { StageManager::Instance().GetStage(StageManager::Instance().GetStageIndex())->GetStageCollision().left_up.x + 0.5f,
         StageManager::Instance().GetStage(StageManager::Instance().GetStageIndex())->GetStageCollision().right_down.x - 0.5f };

    unit_category = 1;
    // とりあえずアニメーション
    model->PlayAnimation(Animation::Idle, true);
}

Player::~Player()
{
}

// 更新処理
void Player::Update(float elapsedTime)
{
    sub_pos_z = StageManager::Instance().GetStage(StageManager::Instance().GetStageIndex())->GetStageCollision().right_down.y - 1.0f;

    // 座標を減算
    position.z = sub_pos_z;

    // 左右端
    limit = { StageManager::Instance().GetStage(StageManager::Instance().GetStageIndex())->GetStageCollision().left_up.x + 0.5f,
         StageManager::Instance().GetStage(StageManager::Instance().GetStageIndex())->GetStageCollision().right_down.x - 0.5f };


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

    // 移動の制限
    if (position.x <= limit.x)
    {
        position.x = limit.x;
    }
    else if (position.x >= limit.y)
    {
        position.x = limit.y;
    }
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
        model->PlayAnimation(Animation::Pull, false);
        velocity.x = 0;

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
            model->PlayAnimation(Animation::Throw, false);
            velocity.x = 0;

            // はじき距離を算出
            f_d = (s_l_max) / timer_s * 2.0f;
            // 初期化
            s_l_max = 0;
            timer_s = 0;

            // スケーリング
            float scaling = StageManager::Instance().GetStage(StageManager::Instance().GetStageIndex())->GetVariableStageWidth().y * 2;
            f_d = scaling * (f_d / scaling);
            // 最小値0、最大値scalingにクランプする
            f_d = std::clamp(f_d, 0.2f, scaling);
        }
    }

    //TODO この条件は暴発しかねないので要修正
    if (f_d >= 0.1f)
    {
        velocity.x = 0;
        Seed* seed = new Seed();
        DirectX::XMFLOAT2 unit_pos;
        if (!CollisionManager::CollisionUnitBackVsSeed_Re({ position.x ,f_d + sub_pos_z/*はじきで出た座標から、ステージの半径を減算*/ }, unit_pos))
        {
            Seed* seed = new Seed();

            // 座標を確定
            seed->SetPosition(position.x/*プレイヤーのX座標*/, 0, position.z);
            seed->SetDistinationPosition({ position.x/*プレイヤーのX座標*/, 0, f_d + sub_pos_z });

            // ユニットがいないなら即座に発芽
            if (UnitManager::Instance().GetUnitCount() == 0)    seed->SetBorn(true);

            // 種の種類を登録
            seed->SetCategory(unit_category);

            seed->SetIsDirection(true);

            seed->UpdateTransform();

            // リストに追加
            SeedManager::Instance().Register(seed);
            // はじき距離を初期化
            f_d = 0.0f;
        }
        else
        {
            // 座標を確定
            seed->SetPosition(position.x/*プレイヤーのX座標*/, 0, position.z);
            seed->SetDistinationPosition({ position.x/*プレイヤーのX座標*/, 0, unit_pos.y });

            seed->UpdateTransform();

            seed->SetIsDisDirection(true);
            // リストに追加
            SeedManager::Instance().Register(seed);
            f_d = 0.0f;
        }
    }

}

// 入力処理
void Player::InputProcess()
{
    GamePad& gamePad = Input::Instance().GetGamePad();
    if ((model->GetCurrentAnimationIndex() == Animation::Throw|| model->GetCurrentAnimationIndex() == Animation::Pull) && model->IsPlayAnimation())
    {
        velocity.x = 0;
    }
    else
    {
        // 左スティックX成分をスピードに変換
        velocity.x = gamePad.GetAxisLX() * moveSpeed;
        if (velocity.x < 0)  model->PlayAnimation(Animation::Left, false);
        else if (velocity.x > 0)  model->PlayAnimation(Animation::Right, false);
        else if (velocity.x == 0)  model->PlayAnimation(Animation::Idle, false);
    }
}

Player& Player::Instance()
{
    return *instance;
}
