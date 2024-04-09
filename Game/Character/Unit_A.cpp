#include "Unit_A.h"
#include "EnemyManager.h"
#include "UnitManager.h"
#include "Lemur/Input/Input.h"
#include "Lemur/Graphics/DebugRenderer.h"
#include "Lemur/Collision/Collision.h"
#include "interval.h"

Unit_A::Unit_A()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Unit\\Chili_24_0305_01.fbx");

    attack_times = 5;// 攻撃回数
    attack_power = 1;// 攻撃力
    attack_interval = 0.5f;// 攻撃間隔
    attack_radius = 1.0f;// 攻撃範囲
    radius = 0.3f;// 半径
    height = 0.5f;// デバッグ用
    dec_pos = 1.0f;// 

    // とりあえずアニメーション
    model->PlayAnimation(0, true);
}

Unit_A::~Unit_A()
{
}

void Unit_A::Update(float elapsedTime)
{
    // 速力処理更新
    UpdateVelocity(elapsedTime);

    // 無的時間更新
    UpdateInvincibleTimer(elapsedTime);

    // Transform の更新
    UpdateTransform();

    // モデルアニメーション更新処理
    model->UpdateAnimation(elapsedTime);

    DrawDebugGUI();

    AttackEnemy(elapsedTime);
}

void Unit_A::Render(float scale, ID3D11PixelShader** replaced_pixel_shader)
{
    model->Render(scale, replaced_pixel_shader);
}

void Unit_A::AttackEnemy(float elapsedTime)
{
    EnemyManager& enemyManager = EnemyManager::Instance();

    int enemyCount = enemyManager.GetEnemyCount();

    //TODO モーションが来たらまた変える
    // 敵の総当たり
    for (int j = 0; j < enemyCount; ++j)
    {
        Enemy* enemy = enemyManager.GetEnemy(j);

        // 敵がユニットの攻撃範囲に入っているとき
        if (Collision::IntersectCircleVsCircle
        (
            {position.x,position.z},                // ユニットの位置(XZ平面)
            attack_radius,                          // 攻撃範囲
            {enemy->position.x,enemy->position.z},  // 敵の位置(XZ平面)
            enemy->radius                           // 敵の当たり判定
        ))
        {
            attack_timer += elapsedTime;

            if (attack_timer >= attack_interval)
            {
                if (enemy->ApplyDamage(attack_power))
                {
                    attack_timer = 0.0f;
                    attack_times--;
                }
            }
        }
    }

    // 攻撃回数を消費しきったら消滅
    if (attack_times <= 0)
    {
        UnitManager::Instance().Remove(this);
    }
}

void Unit_A::DrawDebugGUI()
{
    //TODO 同じ種類複製するとやばい
    model->DrawDebug("Unit_A");
}

void Unit_A::DrawDebugPrimitive()
{
    DebugRenderer* debug_renderer = Lemur::Graphics::Graphics::Instance().GetDebugRenderer();
    debug_renderer->DrawCylinder(position, attack_radius, height, { 1,0,0,1 });
    debug_renderer->DrawCylinder(position, radius, height, { 0,1,0,1 });
}

