#include "Enemy_ABCD.h"
#include "EnemyManager.h"
#include "UnitManager.h"

//------EnemyA-----------------------------------------------------------------

Enemy_A::Enemy_A()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Enemy\\spider_v009.fbx");

    attack_power    = 1;        // 攻撃力
    attack_interval = 3.0f;     // 攻撃間隔

    health          = 10;       // HP
    radius          = 1.0f;     // 半径
    height          = 1.0f;     // デバッグ用
    speed_power     = -1.0f;    // 速度


    // とりあえずアニメーション
    model->PlayAnimation(Animation::Move, true);
}

void Enemy_A::DrawDebugPrimitive()
{
    DebugRenderer* debug_renderer = Lemur::Graphics::Graphics::Instance().GetDebugRenderer();
    debug_renderer->DrawCylinder(position, radius, height, { 0,1,0,1 });
}

void Enemy_A::DrawDebugGUI(int n)
{
    Character::DrawDebugGUI("Enemy_A", n);
}

void Enemy_A::UpdateAttackState(float elapsed_time)
{
    // タイマーを加算
    timer += elapsed_time;

    // 攻撃間隔とタイマーを比べる
    if (timer >= attack_interval)
    {
         // 柵に当たったら
        if (Fence::Instance().ApplyDamage(attack_power))
        {
            //TODO これはアニメーションがきたら要変更
            model->PlayAnimation(Animation::Attack, false);
            // タイマーをに
            timer = 0.0f;
        }
    }

    //TODO これはアニメーションがきたら要変更
    if (!model->IsPlayAnimation())
    {
        model->PlayAnimation(7, true);
    }

    // 死亡時は死亡ステートへ切り替え
    if (IsDead())
    {
        TransitionDeathState();
    }
}

void Enemy_A::UpdateMoveState(float elapsed_time)
{
    if (shaft == Shaft::Side) rotation.y = DirectX::XMConvertToRadians(-90);
    else if (shaft == Shaft::Vertical)rotation.y = DirectX::XMConvertToRadians(180);

    // 柵に触れれば攻撃ステートへ
    if (Collision::IntersectRectVsCircle(Fence::Instance().GetLeftRect(), { position.x,position.z }, radius)||
        Collision::IntersectRectVsCircle(Fence::Instance().GetFrontRect(), { position.x,position.z }, radius))
    {
        TransitionAttackState();
    }
    else
    {
        // 移動
        if (shaft == Shaft::Side) velocity.x = speed_power;
        else if(shaft==Shaft::Vertical)velocity.z = speed_power;
    }

    // 死亡ステートへ
    if (IsDead())
    {
        TransitionDeathState();
    }
}


//------ EnemyB----------------------------------------------------------------

Enemy_B::Enemy_B()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Enemy\\spider_v009.fbx");

    attack_power    = 1;        // 攻撃力
    attack_interval = 3.0f;     // 攻撃間隔

    radius          = 1.0f;     // 半径
    height          = 1.0f;     // デバッグ用

    dis             = 3.0f;     // 一気に進む距離
    move_timer      = 0.0f;     // タイマー
    move_timer_max  = 3.0f;     // 動くまでの時間
    speed_power     = -3.0f;    // スピード

    health          = 10;       // HP

    // とりあえずアニメーション
    model->PlayAnimation(Animation::Move, true);
}

void Enemy_B::DrawDebugGUI(int n)
{
    Character::DrawDebugGUI("Enemy_B", n);
}

void Enemy_B::UpdateAttackState(float elapsed_time)
{
    // タイマーを加算
    attack_timer += elapsed_time;

    // 攻撃間隔とタイマーを比べる
    if (attack_timer >= attack_interval)
    {
        // 柵に当たったら
        if (Fence::Instance().ApplyDamage(attack_power))
        {
            //TODO これはアニメーションがきたら要変更
            model->PlayAnimation(Animation::Attack, false);
            // タイマーをに
            attack_timer = 0.0f;
        }
    }

    //TODO これはアニメーションがきたら要変更
    if (!model->IsPlayAnimation())
    {
        model->PlayAnimation(7, true);
    }

    // 死亡時は死亡ステートへ切り替え
    if (IsDead())
    {
        TransitionDeathState();
    }
}

void Enemy_B::UpdateMoveState(float elapsed_time)
{
    if (shaft == Shaft::Side) rotation.y = DirectX::XMConvertToRadians(-90);
    else if (shaft == Shaft::Vertical)rotation.y = DirectX::XMConvertToRadians(180);
    // 柵に当たったら
    if (Collision::IntersectRectVsCircle(Fence::Instance().GetLeftRect(), { position.x,position.z }, radius) ||
        Collision::IntersectRectVsCircle(Fence::Instance().GetFrontRect(), { position.x,position.z }, radius))
    {
        TransitionAttackState();
    }

    // タイマーを加算
    move_timer += elapsed_time;

    //タイマーが指定秒を超えたとき
    if (move_timer >= move_timer_max)                   
    {
        if (shaft == Shaft::Side)
        {
            // 速度を代入
            velocity.x = speed_power;
            // 既定の距離進んだら
            if (abs(old_position - position.x) >= dis)
            {
                // 再び待機
                move_timer = 0.0f;
            }
        }
        else
        {
            // 速度を代入
            velocity.z = speed_power;
            // 既定の距離進んだら
            if (abs(old_position - position.z) >= dis)
            {
                // 再び待機
                move_timer = 0.0f;
            }
        }
    }
    else
    {
        if (shaft == Shaft::Side)
        {
            // 前位置を変更
            old_position = position.x;
            // 移動
            velocity.x = 0.0f;
        }
        else
        {
            // 前位置を変更
            old_position = position.z;
            // 移動
            velocity.z = 0.0f;
        }
    }

    // 死亡ステートへ
    if (IsDead())
    {
        TransitionDeathState();
    }
}

void Enemy_B::DrawDebugPrimitive()
{
    DebugRenderer* debug_renderer = Lemur::Graphics::Graphics::Instance().GetDebugRenderer();
    debug_renderer->DrawCylinder(position, radius, height, { 0,1,0,1 });
}

//------EnemyC-----------------------------------------------------------------

Enemy_C::Enemy_C()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Enemy\\spider_v009.fbx");

    attack_power    = 1;         // 攻撃力　
    attack_interval = 3.0f;      // 攻撃間隔

    radius          = 1.0f;      // 半径
    height          = 1.0f;      // デバッグ要
    //position.x      = 5.0f;      // 初期位置
    //rotation.y      = -90.0f;    // 初期角度
    speed_power     = -1.0f;     // 速度
    health          = 10;        // HP

    // とりあえずアニメーション
    model->PlayAnimation(Animation::Move, true);
}

void Enemy_C::DrawDebugPrimitive()
{
    DebugRenderer* debug_renderer = Lemur::Graphics::Graphics::Instance().GetDebugRenderer();
    debug_renderer->DrawCylinder(position, radius, height, { 0,1,0,1 });
}

void Enemy_C::DrawDebugGUI(int n)
{
    Character::DrawDebugGUI("Enemy_C", n);
}

void Enemy_C::UpdateAttackState(float elapsed_time)
{
    // タイマーを加算
    timer += elapsed_time;

    // 攻撃間隔とタイマーを比べる
    if (timer >= attack_interval)
    {
        // 柵に当たったら
        if (Fence::Instance().ApplyDamage(attack_power))
        {
            //TODO これはアニメーションがきたら要変更
            model->PlayAnimation(Animation::Attack, false);
            // タイマーをに
            timer = 0.0f;
        }
    }

    //TODO これはアニメーションがきたら要変更
    if (!model->IsPlayAnimation())
    {
        model->PlayAnimation(7, true);
    }

    // 死亡時は死亡ステートへ切り替え
    if (IsDead())
    {
        TransitionDeathState();
    }
}

void Enemy_C::UpdateMoveState(float elapsed_time)
{
    if (shaft == Shaft::Side)UpdateMoveState_S(elapsed_time);
    else UpdateMoveState_V(elapsed_time);
}

void Enemy_C::UpdateMoveState_S(float elapsed_time)
{
    // 柵に触れたら攻撃ステートへ
    if (Collision::IntersectRectVsCircle(Fence::Instance().GetLeftRect(), { position.x,position.z }, radius) ||
        Collision::IntersectRectVsCircle(Fence::Instance().GetFrontRect(), { position.x,position.z }, radius))
    {
        TransitionAttackState();
    }
    else
    {
        switch (move_state)
        {
        case Move::Straight:// 直線
            // 移動
            velocity.x = speed_power;
            // ユニットに触れたらステートを切り替え
            if (is_touched_unit)move_state = Move::Avoid;
            break;

        case Move::Avoid:// ユニットに触った時

            // 現在地から目的地までのベクトルを算出
            DirectX::XMFLOAT2 Vec =
            {
                destination.x - position.x,
                destination.y - position.z
            };
            // 正規化
            Vec = Normalize(Vec);
            // スピードにかける
            Vec = { Vec.x * abs(speed_power),Vec.y * abs(speed_power) };
            // 移動
            velocity.x = Vec.x;
            velocity.z = Vec.y;

            // 目的地に着いたら戻す
            if (position.z >= destination.y)
            {
                // 初期化
                is_touched_unit = false;
                velocity.x = velocity.z = 0.0f;
                move_state = Move::Straight;
            }

            break;
        }
    }
}

void Enemy_C::UpdateMoveState_V(float elapsed_time)
{
}

void Enemy_C::JudgeUnit()
{
    UnitManager& unitManager = UnitManager::Instance();
    int unitCount = unitManager.GetUnitCount();

    // ユニットにの総当たり
    for (int j = 0; j < unitCount; ++j)
    {
        Unit* unit = unitManager.GetUnit(j);

        if (unit->GetCategory() == 0 || unit->GetCategory() == 3)
        {
            // 敵がユニットの攻撃範囲に触れたとき
            if (Collision::IntersectCircleVsCircle
            (
                { unit->GetPosition().x,unit->GetPosition().z },   // ユニットの位置(XZ平面)
                unit->GetAttackCollisionRange(),                   // 攻撃範囲
                { position.x,position.z },                         // 敵の位置(XZ平面)
                radius                                             // 敵の当たり判定
            ))
            {
                is_touched_unit = true;
                // 目的地を決定する
                destination = {
                    unit->GetPosition().x,
                    unit->GetPosition().z + unit->GetAttackCollisionRange() + radius
                };
            }
        }
        else if (unit->GetCategory() == 1 || unit->GetCategory() == 2)
        {
            // 敵がユニットの攻撃範囲に触れたとき
            if (Collision::IntersectTriangleVsCircle
            (
                unit->GetTriangle1(),
                { position.x,position.z },                         // 敵の位置(XZ平面)
                radius                                             // 敵の当たり判定
            ))
            {
                is_touched_unit = true;
                // 目的地を決定する
                destination = {
                    unit->GetPosition().x,
                    unit->GetPosition().z + unit->GetRadius() + radius
                };
            }
            // 敵がユニットの攻撃範囲に触れたとき
            else if (Collision::IntersectTriangleVsCircle
            (
                unit->GetTriangle1(),
                { position.x,position.z },                         // 敵の位置(XZ平面)
                radius                                             // 敵の当たり判定
            ))
            {
                is_touched_unit = true;
                // 目的地を決定する
                destination = {
                    unit->GetPosition().x,
                    unit->GetPosition().z + unit->GetRadius() + radius
                };
            }
        }
        if (unit->GetCategory() == 4 || unit->GetCategory() == 5)
        {
        }
    }
}

//-------EnemyD---------------------------------------------------------------

Enemy_D::Enemy_D()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Enemy\\spider_v009.fbx");

    attack_power    = 1;        // 攻撃力
    attack_interval = 3.0f;     // 攻撃間隔

    radius          = 1.0f;     // 半径
    height          = 1.0f;     //デバッグ用
    //position.x      = 5.0f;     // 初期位置
    //rotation.y      = -90.0f;   // 初期方向

    move_state      = 0;        // 移動状態
    speed_power     = -1.0f;    // 横移動量
    speed_power_Y   = -1.0f;    // 縦移動量
    direction_state = 0;        // 方向の状態
    dis_max         = 1.0f;     // 移動量規定値
    is_last_touched = false;    // 前回柵に触れたか

    health          = 10;       // HP

    // とりあえずアニメーション
    model->PlayAnimation(Animation::Move, true);
}

void Enemy_D::DrawDebugPrimitive()
{
    DebugRenderer* debug_renderer = Lemur::Graphics::Graphics::Instance().GetDebugRenderer();
    debug_renderer->DrawCylinder(position, radius, height, { 0,1,0,1 });
}

void Enemy_D::DrawDebugGUI(int n)
{
    Character::DrawDebugGUI("Enemy_D", n);
}

void Enemy_D::UpdateAttackState(float elapsed_time)
{
    // タイマーを加算
    attack_timer += elapsed_time;
    // 攻撃間隔とタイマーを比べる
    if (attack_timer >= attack_interval)
    {
        // 柵に当たったら
        if (Fence::Instance().ApplyDamage(attack_power))
        {
            //TODO これはアニメーションがきたら要変更
            model->PlayAnimation(Animation::Attack, false);
            // タイマーを0に
            attack_timer = 0.0f;
        }
    }

    //TODO これはアニメーションがきたら要変更
    if (!model->IsPlayAnimation())
    {
        model->PlayAnimation(7, true);
    }

    // 死亡時は死亡ステートへ切り替え
    if (IsDead())
    {
        TransitionDeathState();
    }
}

void Enemy_D::UpdateMoveState(float elapsed_time)
{
    // 左柵に当たったら
    if (Collision::IntersectRectVsCircle(Fence::Instance().GetLeftRect(), { position.x,position.z }, radius))
    {
        TransitionAttackState();
    }

    switch (move_state)
    {
    case Move::Straight:
        // 横移動
        velocity.x = speed_power;
        // 移動量を記録
        dis += abs(speed_power) * elapsed_time;
        // 移動量が規定値を超えたら
        if (dis >= dis_max)
        {
            dis = 0.0f;                             // 移動量を初期化
            if (is_last_touched)direction_state = 0;// 前回柵に触れて居たら移動方向を固定
            else direction_state = rand() % 2;      // 次回の方向を決定
            is_last_touched = false;                // 初期化
            move_state = 1;                         // 移動状態を変化
        }
        break;
    case Move::Diagonal:
        // 横移動
        velocity.x = speed_power;
        // 移動量を記録
        dis += abs(speed_power) * elapsed_time;

        // 移動量が規定値を超えたらor後ろに行ききったら
        if (dis >= dis_max || is_last_touched)
        {
            dis = 0.0f;       // 移動量を初期化
            velocity.z = 0.0f;// 速度を初期化
            move_state = 0;   // 移動状態を変化
        }

        // 縦移動
        switch (direction_state)
        {
        case Direction::Under:
            velocity.z = speed_power_Y;// 縦移動

            // 後ろ方向に振り切っていたら
            if (Collision::IntersectRectVsCircle(Fence::Instance().GetBackRect(), { position.x,position.z }, radius))
            {
                is_last_touched = true;
            }
            break;
        case Direction::Up:
            velocity.z = -speed_power_Y;// 縦移動

            // 後ろ方向に振り切っていたら
            if (Collision::IntersectRectVsCircle(Fence::Instance().GetBackRect(), { position.x,position.z }, radius))
            {
                is_last_touched = true;
            }
            break;
        }
        break;
    }

    // 死亡ステートへ
    if (IsDead())
    {
        TransitionDeathState();
    }
}

