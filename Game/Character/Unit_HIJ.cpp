#include "Unit_HIJ.h"
#include "EnemyManager.h"
#include "Lemur/Audio/AudioManager.h"

Unit_J::Unit_J()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    //LoadGltfModel(graphics.GetDevice(), ".\\resources\\Model_glb\\Unit\\Chili.glb",true);
    LoadFBXModel(graphics.GetDevice(), ".\\resources\\Model\\Unit\\Mustard.fbx");

    collision_model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Collision\\donut_collision.fbx");

    attack_effect = new Effect(".\\resources\\Effect\\UNIT1\\UNIT1.efk");
    death_effect = new Effect(".\\resources\\Effect\\UNIT_DEATH\\UNIT_DEATH.efk");
    set_effect = new Effect(".\\resources\\Effect\\UNIT_SET\\UNIT_SET.efk");

    attack_times = 5;    // 攻撃回数
    attack_power = 1;    // 攻撃力
    attack_interval = 0.5f; // 攻撃間隔
    attack_collision_range = 2.0f; // 攻撃範囲
    attack_radius_in = 1.0f; // 攻撃範囲
    radius = 0.4f; // 半径
    height = 0.5f; // デバッグ用
    dec_pos = 1.0f; // ユニットに接触した種がどのくらい跳ね返されるか

    UpdateTransform();

    // とりあえずアニメーション
    PlayAnimation(Animation::Idle, true);
    Lemur::Audio::AudioManager::Instance().PlaySe(Lemur::Audio::SE::APPEARANCE, false);
}

void Unit_J::DrawDebugPrimitive()
{
    DebugRenderer* debug_renderer = Lemur::Graphics::Graphics::Instance().GetDebugRenderer();
 /*   debug_renderer->DrawCylinder(position, attack_collision_range, height, { 1,0,1,1 });
    debug_renderer->DrawCylinder(position, attack_radius_in, height, { 1,0,1,1 });
    debug_renderer->DrawCylinder(position, radius, height, { 0,1,0,1 });*/
}

void Unit_J::UpdateIdleState(float elapsed_time)
{
    EnemyManager& enemyManager = EnemyManager::Instance();

    int enemyCount = enemyManager.GetEnemyCount();

    //TODO モーションが来たらまた変える
    // 敵の総当たり
    for (int j = 0; j < enemyCount; ++j)
    {
        Enemy* enemy = enemyManager.GetEnemy(j);

        // ユニットが死んでたらコンティニュー
        if (enemy->IsDead())continue;

        // 敵がユニットの攻撃範囲に入っているとき
        if (Collision::IntersectDonutVsCircle
        (
            { position.x,position.z },                          // ユニットの位置(XZ平面)
            attack_collision_range,    
            attack_radius_in,
            { enemy->GetPosition().x,enemy->GetPosition().z },  // 敵の位置(XZ平面)
            enemy->GetRadius()                                  // 敵の当たり判定
        ))
        {
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

void Unit_J::UpdateAttackState(float elapsed_time)
{
    EnemyManager& enemyManager = EnemyManager::Instance();
    int enemyCount = enemyManager.GetEnemyCount();
    bool is_intersected = false;

    attack_timer += elapsed_time;// 攻撃タイマー
    // タイマーが規定時間を超えたら攻撃
    if (attack_timer >= attack_interval)is_attack = true;

    if (attack_times > 0)// 攻撃回数が残っているとき
    {
        // 敵の総当たり
        for (int j = 0; j < enemyCount; ++j)
        {
            Enemy* enemy = enemyManager.GetEnemy(j);

            // ユニットが死んでたらコンティニュー
            if (enemy->IsDead())continue;

            // ユニットの攻撃範囲に入っている敵全員に攻撃
            if (Collision::IntersectDonutVsCircle
            (
                { position.x,position.z },                          // ユニットの位置(XZ平面)
                attack_collision_range,
                attack_radius_in,
                { enemy->GetPosition().x,enemy->GetPosition().z },  // 敵の位置(XZ平面)
                enemy->GetRadius()                                  // 敵の当たり判定
            ))
            {
                // 敵とかぶったフラグをON
                is_intersected = true;

                if (is_attack)
                {
                    Lemur::Audio::AudioManager::Instance().PlaySe(Lemur::Audio::SE::UNIT_ATTACK, false);
                    // アニメーションの切り替え
                    PlayAnimation(Animation::Attack, false);
                    // 攻撃フラグがONならダメージ処理
                    enemy->ApplyDamage(ReturnDamage());
                    // エフェクトの再生
                    attack_handle = attack_effect->Play(position, attack_effect_size);
                }
            }
        }

        // 範囲内に敵が一体も居なければ待機
        if (!is_intersected)    TransitionIdleState();
        else// 誰か一体でも範囲内にいる場合
        {
            // 攻撃中なら残り攻撃回数を減らしタイマーを初期化
            if (is_attack)
            {
                is_attack = false;
                attack_timer = 0.0f;
                attack_times--;
            }
        }
    }
    else
    {
        // 攻撃回数を消費しきったら消滅
        if (!IsPlayAnimation()) TransitionDeathState();
    }
}

void Unit_J::DrawDebugGUI(int n)
{
    Character::DrawDebugGUI("Unit_J", n);
}
