#include "Enemy_ABCD.h"
#include "EnemyManager.h"
#include "UnitManager.h"

//------EnemyA-----------------------------------------------------------------

Enemy_A::Enemy_A()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Enemy\\enemy_01.fbx");

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
    model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Enemy\\enemy_02.fbx");

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
    model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Enemy\\enemy_03.fbx");

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
    if (shaft == Shaft::Side)
    {
        // 移動
        HandleMovementState(Fence::Instance().GetLeftRect(), Fence::Instance().GetFrontRect(),
            speed_power, Move::Straight, Move::Avoid, velocity.x, is_touched_unit, elapsed_time);
        //
        JudgeUnit(true);
    }
    else
    {
        // 移動
        HandleMovementState(Fence::Instance().GetLeftRect(), Fence::Instance().GetFrontRect(),
            speed_power, Move::Straight, Move::Avoid, velocity.z, is_touched_unit, elapsed_time);
        //
        JudgeUnit(false);
    }

    if (IsDead())
    {
        TransitionDeathState();
    }
}

void Enemy_C::JudgeUnit(bool isVertical)
{
    UnitManager& unitManager = UnitManager::Instance();
    int unitCount = unitManager.GetUnitCount();

    // ユニットにの総当たり
    for (int j = 0; j < unitCount; ++j)
    {
        Unit* unit = unitManager.GetUnit(j);

        // 円
        if (unit->GetCategory() == 0 || unit->GetCategory() == 3) 
        {
            if (Collision::IntersectCircleVsCircle(
                { unit->GetPosition().x, unit->GetPosition().z }, // ユニットの位置(XZ平面)
                unit->GetAttackCollisionRange(),                   // 攻撃範囲
                { position.x, position.z },                        // 敵の位置(XZ平面)
                radius))                                           // 敵の当たり判定
            {
                is_touched_unit = true; // ユニットに触れた
                if (isVertical)
                {
                    destination = {
                        unit->GetPosition().x,
                        unit->GetPosition().z + unit->GetAttackCollisionRange() + radius
                    };
                }
                else
                {
                    destination = {
                        // ユニット左に避ける
                        unit->GetPosition().x - radius - unit->GetAttackCollisionRange() ,
                        unit->GetPosition().z
                    };
                }
            }
        }
        else if (unit->GetCategory() == 1) // 三角形
        {
            if (isVertical)
            {
                // 敵がユニットの攻撃範囲に触れたとき
                // 右三角
                if (Collision::IntersectTriangleVsCircle
                (
                    unit->GetTriangle2(),
                    { position.x,position.z },                         // 敵の位置(XZ平面)
                    radius                                             // 敵の当たり判定
                ))
                {
                    is_touched_unit = true;// ユニットに触れた
                    // 目的地を決定する
                    destination = {
                        unit->GetPosition().x,
                        unit->GetPosition().z + unit->GetRadius() + radius
                    };
                }
            }
            else if (!isVertical )
            {
                // 敵がユニットの攻撃範囲に触れたとき
                if (Collision::IntersectTriangleVsCircle// 左三角
                (
                    unit->GetTriangle1(),
                    { position.x,position.z },                         // 敵の位置(XZ平面)
                    radius                                             // 敵の当たり判定
                ))
                {
                    is_touched_unit = true;// ユニットに触れた
                    // 目的地を決定する
                    destination = {
                        // ユニット左三角形の底辺方向に避ける
                        unit->GetPosition().x - unit->GetRadius() - radius - unit->GetTriangleHeight(),
                        unit->GetPosition().z
                    };
                }
                // 敵がユニットの攻撃範囲に触れたとき
                else if (Collision::IntersectTriangleVsCircle// 右三角
                (
                    unit->GetTriangle2(),
                    { position.x,position.z },                         // 敵の位置(XZ平面)
                    radius                                             // 敵の当たり判定
                ))
                {
                    is_touched_unit = true;// ユニットに触れた
                    // 目的地を決定する
                    destination = {
                        // ユニット右三角形の底辺方向に避ける
                        unit->GetPosition().x + unit->GetRadius() + radius + unit->GetTriangleHeight(),
                        unit->GetPosition().z
                    };
                }
            }
        }
        else if (unit->GetCategory() == 2)
        {
            if (isVertical)
            {
                // 敵がユニットの攻撃範囲に触れたとき
          // 奥三角
                if (Collision::IntersectTriangleVsCircle
                (
                    unit->GetTriangle1(),
                    { position.x,position.z },                         // 敵の位置(XZ平面)
                    radius                                             // 敵の当たり判定
                ))
                {
                    is_touched_unit = true;// ユニットに触れた
                    // 目的地を決定する
                    destination = {
                        unit->GetPosition().x,
                        unit->GetPosition().z + unit->GetRadius() + radius + unit->GetTriangleHeight()
                    };
                }
                // 敵がユニットの攻撃範囲に触れたとき
                // 手前三角
                else if (Collision::IntersectTriangleVsCircle
                (
                    unit->GetTriangle2(),
                    { position.x,position.z },                         // 敵の位置(XZ平面)
                    radius                                             // 敵の当たり判定
                ))
                {
                    is_touched_unit = true;// ユニットに触れた
                    // 目的地を決定する
                    destination = {
                        unit->GetPosition().x,
                        unit->GetPosition().z - unit->GetRadius() - radius - unit->GetTriangleHeight()
                    };
                }
            }
            else if (!isVertical)
            {
                // 敵がユニットの攻撃範囲に触れたとき
                if (Collision::IntersectTriangleVsCircle// 奥三角
                (
                    unit->GetTriangle1(),
                    { position.x,position.z },                         // 敵の位置(XZ平面)
                    radius                                             // 敵の当たり判定
                ))
                {
                    is_touched_unit = true;// ユニットに触れた
                    // 目的地を決定する
                    destination = {
                        // ユニット左に避ける
                        unit->GetPosition().x - unit->GetRadius() - radius,
                        unit->GetPosition().z
                    };
                }
            }
        }
        else if (unit->GetCategory() == 4 && isVertical) // 長方形
        {
            // 敵がユニットの攻撃範囲に触れたとき
            if (Collision::IntersectRectVsCircle
            (
                unit->GetAttackRect(),
                { position.x,position.z },                         // 敵の位置(XZ平面)
                radius                                             // 敵の当たり判定
            ))
            {
                is_touched_unit = true;// ユニットに触れた
                // 目的地を決定する
                destination = {
                    unit->GetPosition().x,
                    unit->GetPosition().z + unit->GetRadius() + radius
                };
            }
        }
        else if (unit->GetCategory() == 5 && !isVertical)
        {
            // 敵がユニットの攻撃範囲に触れたとき
            if (Collision::IntersectRectVsCircle
            (
                unit->GetAttackRect(),
                { position.x,position.z },                         // 敵の位置(XZ平面)
                radius                                             // 敵の当たり判定
            ))
            {
                is_touched_unit = true;// ユニットに触れた
                // 目的地を決定する
                destination = {
                    unit->GetPosition().x - unit->GetRadius() - radius,
                    unit->GetPosition().z
                };
            }
        }
    }
}

void Enemy_C::HandleMovementState(const Rect& rect1, const Rect& rect2, float speedPower, Move straightState, Move avoidState, float& velocityComponent, bool& touchedUnit, float elapsed_time)
{
    // 柵に触れたら
    if (CheckFenceCollision(rect1, rect2))
    {
        // 攻撃ステートへ
        TransitionAttackState();
    }
    else
    {
        switch (move_state)
        {
        case Move::Straight:
            // まっすぐ動かす
            velocityComponent = speedPower;
            // ユニットに触れたらステートを変更
            if (touchedUnit) move_state = avoidState;
            break;

        case Move::Avoid:
            // 敵を目的地まで動かす
            MoveToDestination(elapsed_time, straightState);
            break;
        }
    }
}

void Enemy_C::MoveToDestination(float elapsed_time, Move nextState)
{
    DirectX::XMFLOAT2 destVec = { destination.x - position.x, destination.y - position.z };
    destVec = Normalize(destVec);

    destVec = { destVec.x * abs(speed_power), destVec.y * abs(speed_power) };

    velocity.x = destVec.x;
    velocity.z = destVec.y;

    if (ReachedDestination())
    {
        ResetMovementState(nextState);// 移動状態のリセット
    }
}

//-------EnemyD---------------------------------------------------------------

Enemy_D::Enemy_D()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Enemy\\enemy_04.fbx");

    attack_power    = 1;        // 攻撃力
    attack_interval = 3.0f;     // 攻撃間隔

    radius          = 1.0f;     // 半径
    height          = 1.0f;     // デバッグ用

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

void Enemy_D::UpdateMoveState_S(float elapsed_time)
{
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

void Enemy_D::UpdateMoveState_V(float elapsed_time)
{
    switch (move_state)
    {
    case Move::Straight:
        // 縦移動
        velocity.z = speed_power;
        // 移動量を記録
        dis += abs(speed_power) * elapsed_time;
        // 移動量が規定値を超えたら
        if (dis >= dis_max)
        {
            dis = 0.0f;                             // 移動量を初期化
            if (is_last_touched)direction_state = 0;// 前回柵に触れて居たら移動方向を固定
            else direction_state = rand() % 2;      // 次回の方向を決定
            is_last_touched = false;                // 初期化
            move_state = Move::Diagonal;            // 移動状態を変化
        }
        break;
    case Move::Diagonal:
        // 縦移動
        velocity.z = speed_power;
        // 移動量を記録
        dis += abs(speed_power) * elapsed_time;

        // 移動量が規定値を超えたらor右に行ききったら
        if (dis >= dis_max || is_last_touched)
        {
            dis = 0.0f;                     // 移動量を初期化
            velocity.x = 0.0f;              // 速度を初期化
            move_state = Move::Straight;    // 移動状態を変化
        }

        // 縦移動
        switch (direction_state)
        {
        case Direction::Under:// 右側
            velocity.x = speed_power_Y;// 右斜め移動

            // 後ろ方向に振り切っていたら
            if (Collision::IntersectRectVsCircle(Fence::Instance().GetRightRect(), { position.x,position.z }, radius))
            {
                is_last_touched = true;
            }
            break;
        case Direction::Up:// 左側
            velocity.x = -speed_power_Y;// 縦移動

            // 後ろ方向に振り切っていたら
            if (Collision::IntersectRectVsCircle(Fence::Instance().GetRightRect(), { position.x,position.z }, radius))
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

void Enemy_D::UpdateMoveState(float elapsed_time)
{
    // 柵に当たったら
    if (Collision::IntersectRectVsCircle(Fence::Instance().GetLeftRect(), { position.x,position.z }, radius)||
        Collision::IntersectRectVsCircle(Fence::Instance().GetFrontRect(), { position.x,position.z }, radius))
    {
        TransitionAttackState();
    }

    if (shaft == Shaft::Side)UpdateMoveState_S(elapsed_time);
    else    UpdateMoveState_V(elapsed_time);
 
}

