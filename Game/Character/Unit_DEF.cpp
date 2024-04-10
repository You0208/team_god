#include "Unit_DEF.h"
#include "UnitManager.h"

//--------Unit_D--------------------------------------------------

Unit_D::Unit_D()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Unit\\Chili_24_0305_01.fbx");

    attack_times           = 5;    // 攻撃回数
    attack_power           = 1;    // 攻撃力
    attack_interval        = 0.5f; // 攻撃間隔
    attack_collision_range = 1.0f; // 攻撃範囲
    radius                 = 0.3f; // 半径
    height                 = 0.5f; // デバッグ用
    dec_pos                = 1.0f; // ユニットに接触した種がどのくらい跳ね返されるか

    // とりあえずアニメーション
    model->PlayAnimation(Animation::Idle, true);
}

void Unit_D::DrawDebugPrimitive()
{
    DebugRenderer* debug_renderer = Lemur::Graphics::Graphics::Instance().GetDebugRenderer();
    debug_renderer->DrawCylinder(position, attack_collision_range, height, { 1,0,0,1 });
    debug_renderer->DrawCylinder(position, radius, height, { 0,1,0,1 });
}

void Unit_D::UpdateIdleState(float elapsed_time)
{
    UnitManager& unitManager = UnitManager::Instance();
    int unitCount = unitManager.GetUnitCount();

    //TODO モーションが来たらまた変える
    // ユニットの総当たり
    for (int j = 0; j < unitCount; ++j)
    {
        Unit* unit = unitManager.GetUnit(j);

        // 他のユニットがユニットの攻撃範囲に入っているとき
        if (Collision::IntersectCircleVsCircle
        (
            { position.x,position.z },                // ユニットの位置(XZ平面)
            attack_collision_range,                          // 攻撃範囲
            { unit->GetPosition().x,unit->GetPosition().z },  // 敵の位置(XZ平面)
            unit->GetRadius()                           // 敵の当たり判定
        ))
        {
            buff_unit = unit;
            // 攻撃ステートに切り替え
            TransitionAttackState();
        }
    }
    // 攻撃回数を消費しきったら消滅
    if (attack_times <= 0)
    {
        TransitionDeathState();
    }
}

void Unit_D::UpdateAttackState(float elapsed_time)
{
    attack_timer += elapsed_time;// 攻撃タイマー

    if (attack_timer >= attack_interval)
    {
        // バフをかける
        attack_timer = 0.0f;
        attack_times--;
    }

    UnitManager& unitManager = UnitManager::Instance();
    int unitCount = unitManager.GetUnitCount();
    bool is_intersected = false;

    //TODO モーションが来たらまた変える
    // ユニットの総当たり
    for (int j = 0; j < unitCount; ++j)
    {
        Unit* unit = unitManager.GetUnit(j);

        // 他のユニットがユニットの攻撃範囲に入っているとき
        if (Collision::IntersectCircleVsCircle
        (
            { position.x,position.z },                // ユニットの位置(XZ平面)
            attack_collision_range,                          // 攻撃範囲
            { unit->GetPosition().x,unit->GetPosition().z },  // 敵の位置(XZ平面)
            unit->GetRadius()                           // 敵の当たり判定
        ))
        {
            is_intersected = true;
            break;
        }
    }

    // 範囲内に敵が一体も居なければ待機
    if (!is_intersected)    TransitionIdleState();

    // 攻撃回数を消費しきったら消滅
    if (attack_times <= 0)
    {
        TransitionDeathState();
    }
}

void Unit_D::DrawDebugGUI(int n)
{
    Character::DrawDebugGUI("Unit_D", n);
}


//--------Unit_E--------------------------------------------------

Unit_E::Unit_E()
{
}

void Unit_E::DrawDebugPrimitive()
{
}

void Unit_E::UpdateIdleState(float elapsed_time)
{
}

void Unit_E::UpdateAttackState(float elapsed_time)
{
}

void Unit_E::DrawDebugGUI(int n)
{
}


//--------Unit_F--------------------------------------------------

Unit_F::Unit_F()
{
}

void Unit_F::DrawDebugPrimitive()
{
}

void Unit_F::UpdateIdleState(float elapsed_time)
{
}

void Unit_F::UpdateAttackState(float elapsed_time)
{
}

void Unit_F::DrawDebugGUI(int n)
{
}
