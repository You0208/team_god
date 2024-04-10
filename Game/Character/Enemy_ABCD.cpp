#include "Enemy_ABCD.h"
#include "EnemyManager.h"
#include "UnitManager.h"

//------EnemyA-----------------------------------------------------------------

Enemy_A::Enemy_A()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Enemy\\spider_v009.fbx");

    attack_power = 1;     // 攻撃力
    attack_interval = 3.0f;  // 攻撃間隔

    health = 10;    // HP
    radius = 1.0f;  // 半径
    height = 1.0f;  // デバッグ用
    position.x = 5.0f;  // 初期位置
    rotation.y = -90.0f;// 初期角度
    speed_power = -1.0f; // 速度

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
    // 柵に触れれば攻撃ステートへ
    if (Collision::IntersectRectVsCircle(Fence::Instance().left_rect, { position.x,position.z }, radius))
    {
        TransitionAttackState();
    }
    else
    {
        // 移動
        velocity.x = speed_power;
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

    attack_power = 1;        // 攻撃力
    attack_interval = 3.0f;     // 攻撃間隔

    radius = 1.0f;     // 半径
    height = 1.0f;     // デバッグ用
    position.x = 5.0f;     // 初期位置
    rotation.y = -90.0f;   // 初期方向

    dis = 3.0f;     // 一気に進む距離
    move_timer = 0.0f;     // タイマー
    move_timer_max = 3.0f;     // 動くまでの時間
    speed_power = -3.0f;    // スピード

    health = 10;       // HP
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
    // タイマーを加算
    move_timer += elapsed_time;

    if (move_timer >= move_timer_max)                   //タイマーが4秒を超えたとき
    {
        velocity.x = speed_power;                       // 速度を代入
        if (abs(old_position - position.x) >= dis)      // 既定の距離進んだら
        {
            move_timer = 0.0f;                          // 再び待機
        }
    }
    // 柵に当たったら
    else if (Collision::IntersectRectVsCircle(Fence::Instance().left_rect, { position.x,position.z }, radius))
    {
        TransitionAttackState();
    }
    else
    {
        // 前位置を変更
        old_position = position.x;
        // 移動
        velocity.x = 0.0f;
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

    attack_power = 1;         // 攻撃力　
    attack_interval = 3.0f;      // 攻撃間隔

    radius = 1.0f;      // 半径
    height = 1.0f;      // デバッグ要
    position.x = 5.0f;      // 初期位置
    rotation.y = -90.0f;    // 初期角度
    speed_power = -1.0f;     // 速度
    health = 10;        // HP

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
    // 柵に触れたら
    if (Collision::IntersectRectVsCircle(Fence::Instance().left_rect, { position.x,position.z }, radius) ||
        Collision::IntersectRectVsCircle(Fence::Instance().front_rect, { position.x,position.z }, radius))
    {
        TransitionAttackState();
    }
    else
    {
        switch (move_state)
        {
        case 0:
            // 移動
            velocity.x = speed_power;
            if (is_touched_unit)move_state = 1;
            break;
        case 1:// ユニットに触った時
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
                is_touched_unit = false;
                velocity.x = velocity.z = 0.0f;
                move_state = 0;
            }

            break;
        }
    }
}

void Enemy_C::JudgeUnit()
{
    UnitManager& unitManager = UnitManager::Instance();

    int unitCount = unitManager.GetUnitCount();

    // 敵の総当たり
    for (int j = 0; j < unitCount; ++j)
    {
        Unit* unit = unitManager.GetUnit(j);

        // 敵がユニットの攻撃範囲に触れたとき
        if (Collision::IntersectCircleVsCircle
        (
            { unit->GetPosition().x,unit->GetPosition().z },   // ユニットの位置(XZ平面)
            unit->GetAttackRadius(),                 // 攻撃範囲
            { position.x,position.z },               // 敵の位置(XZ平面)
            radius                                   // 敵の当たり判定
        ))
        {
            is_touched_unit = true;
            destination = {
                unit->GetPosition().x,
                unit->GetPosition().z + unit->GetRadius() + radius
            };
        }
    }
}

//-------EnemyD---------------------------------------------------------------

Enemy_D::Enemy_D()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Enemy\\spider_v009.fbx");

    attack_power = 1;        // 攻撃力
    attack_interval = 3.0f;     // 攻撃間隔

    radius = 1.0f;     // 半径
    height = 1.0f;     //デバッグ用
    position.x = 5.0f;     // 初期位置
    rotation.y = -90.0f;   // 初期方向

    move_state = 0;        // 移動状態
    speed_power = -1.0f;    // 横移動量
    speed_power_Y = -1.0f;    // 縦移動量
    direction_state = 0;        // 方向の状態
    dis_max = 1.0f;     // 移動量規定値
    is_last_touched = false;    // 前回柵に触れたか

    health = 10;       // HP

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
    if (Collision::IntersectRectVsCircle(Fence::Instance().left_rect, { position.x,position.z }, radius))
    {
        TransitionAttackState();
    }

    switch (move_state)
    {
    case 0:
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
    case 1:
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
        case 0:
            velocity.z = speed_power_Y;// 縦移動

            // 後ろ方向に振り切っていたら
            if (Collision::IntersectRectVsCircle(Fence::Instance().back_rect, { position.x,position.z }, radius))
            {
                is_last_touched = true;
            }
            break;
        case 1:
            velocity.z = -speed_power_Y;// 縦移動

            // 後ろ方向に振り切っていたら
            if (Collision::IntersectRectVsCircle(Fence::Instance().back_rect, { position.x,position.z }, radius))
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

