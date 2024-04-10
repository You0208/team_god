#include "Unit_D.h"
#include "UnitManager.h"
#include "Lemur/Input/Input.h"
#include "Lemur/Graphics/DebugRenderer.h"
#include "Lemur/Collision/Collision.h"

Unit_D::Unit_D()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Unit\\Chili_24_0305_01.fbx");

    attack_times    = 5;    // 攻撃回数
    attack_power    = 1;    // 攻撃力
    attack_interval = 0.5f; // 攻撃間隔
    attack_radius   = 1.0f; // 攻撃範囲
    radius          = 0.3f; // 半径
    height          = 0.5f; // デバッグ用
    dec_pos         = 1.0f; // ユニットに接触した種がどのくらい跳ね返されるか

    // とりあえずアニメーション
    model->PlayAnimation(0, true);
}

Unit_D::~Unit_D()
{
}

void Unit_D::Update(float elapsedTime)
{
    AttackEnemy(elapsedTime);

    // 速力処理更新
    UpdateVelocity(elapsedTime);

    // 無的時間更新
    UpdateInvincibleTimer(elapsedTime);

    // Transform の更新
    UpdateTransform();

    // モデルアニメーション更新処理
    model->UpdateAnimation(elapsedTime);

    DrawDebugGUI();
}

void Unit_D::AttackEnemy(float elapsedTime)
{
    UnitManager& unitManager = UnitManager::Instance();

    int unitCount = unitManager.GetUnitCount();

    //TODO モーションが来たらまた変える
    // 敵の総当たり
    for (int j = 0; j < unitCount; ++j)
    {
        Unit* unit = unitManager.GetUnit(j);

        // ユニットがユニットの攻撃範囲に入っているとき
        if (Collision::IntersectCircleVsCircle
        (
            { position.x,position.z },                // ユニットの位置(XZ平面)
            attack_radius,                          // 攻撃範囲
            { unit->GetPosition().x,unit->GetPosition().z },  // 敵の位置(XZ平面)
            unit->GetRadius()                           // 敵の当たり判定
        ))
        {
            attack_timer += elapsedTime;

            if (attack_timer >= attack_interval)
            {
                //TODO バフ
            }
        }
    }

    // 攻撃回数を消費しきったら消滅
    if (attack_times <= 0)
    {
        UnitManager::Instance().Remove(this);
    }
}

void Unit_D::DrawDebugGUI()
{
    model->DrawDebug("Unit_D");
}

void Unit_D::DrawDebugPrimitive()
{
    DebugRenderer* debug_renderer = Lemur::Graphics::Graphics::Instance().GetDebugRenderer();
    debug_renderer->DrawCylinder(position, attack_radius, height, { 1,0,0,1 });
    debug_renderer->DrawCylinder(position, radius, height, { 0,1,0,1 });
}

void Unit_D::TransitionIdleState()
{
}

void Unit_D::UpdateIdleState(float elapsed_time)
{
}

void Unit_D::TransitionAttackState()
{
}

void Unit_D::UpdateAttackState(float elapsed_time)
{
}
