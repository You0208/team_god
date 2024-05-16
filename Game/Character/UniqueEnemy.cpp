#include "UniqueEnemy.h"
#include "Enemy_ABCD.h"
#include "EnemyManager.h"
#include "Lemur/Input/Input.h"

SummonEnemy::SummonEnemy(bool is_minor)
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    if (!is_minor)LoadFBXModel(graphics.GetDevice(), ".\\resources\\Model\\Enemy\\Bear.fbx");
    else LoadFBXModel(graphics.GetDevice(), ".\\resources\\Model\\Enemy\\Bear2.fbx");

    death_effect = new Effect(".\\resources\\Effect\\ENEMY_DOWN\\ENEMY_DOWN.efk");
    hit_effect = new Effect(".\\resources\\Effect\\hit_to_enemy\\hit_to_enemy.efk");

    attack_power = 1;        // 攻撃力
    attack_interval = 7.0f;     // 攻撃間隔

    health = 10;       // HP

    height = 1.0f;     // デバッグ用
    speed_power = -1.0f;    // 速度

    // TODO もね 出現敵の大きさ
    child_scale = 0.95f;

    // TODO もね 召喚敵ランダムステータス
  
    // 出現敵のステータススタック
    status[0].attack_power = 1;              //攻撃力
    status[0].attack_interval = 2.6f;        //攻撃間隔
    status[0].health = 5;                    // HP
    status[0].speed_power = -1.70f;            // 速度
    status[0].radius = 0.5f;                 // 半径
    status[0].attack_effect_size = 0.3f;     // 攻撃エフェクトのサイズ
    status[0].death_effect_size = 0.5f;      // 死亡エフェクトのサイズ
    status[0].hit_effect_size = 0.5f;        // 設置エフェクトのサイズ

    status[1].attack_power = 2;              //攻撃力
    status[1].attack_interval = 2.8f;        //攻撃間隔
    status[1].health = 6;                    // HP
    status[1].speed_power = -1.67f;           // 速度
    status[1].radius = 0.5f;                 // 半径
    status[1].attack_effect_size = 0.3f;     // 攻撃エフェクトのサイズ
    status[1].death_effect_size = 0.5f;      // 死亡エフェクトのサイズ
    status[1].hit_effect_size = 0.5f;        // 設置エフェクトのサイズ

    status[2].attack_power = 3;              //攻撃力
    status[2].attack_interval = 3.0f;        //攻撃間隔
    status[2].health = 3;                    // HP
    status[2].speed_power = -1.79f;            // 速度
    status[2].radius = 0.5f;                 // 半径
    status[2].attack_effect_size = 0.3f;     // 攻撃エフェクトのサイズ
    status[2].death_effect_size = 0.5f;      // 死亡エフェクトのサイズ
    status[2].hit_effect_size = 0.5f;        // 設置エフェクトのサイズ

    status[3].attack_power = 4;              //攻撃力
    status[3].attack_interval = 3.8f;        //攻撃間隔
    status[3].health = 4;                    // HP
    status[3].speed_power = -1.75f;           // 速度
    status[3].radius = 0.5f;                 // 半径
    status[3].attack_effect_size = 0.3f;     // 攻撃エフェクトのサイズ
    status[3].death_effect_size = 0.5f;      // 死亡エフェクトのサイズ
    status[3].hit_effect_size = 0.5f;        // 設置エフェクトのサイズ


    // EnemyB
    mover_timer_max_B = 1.0f; // 一気に進むまでの時間
    dis_B             = 3.0f;     // 一気に進む距離

    // TODO もね　summon召喚物大きさ
    model_scale = 0.9f;
    // 半径の調整
    radius = 1.0f;     // 半径

    // とりあえずアニメーション
    PlayAnimation(Animation::Attack, false);
}

void SummonEnemy::DrawDebugPrimitive()
{
    DebugRenderer* debug_renderer = Lemur::Graphics::Graphics::Instance().GetDebugRenderer();
    //debug_renderer->DrawCylinder(position, radius, height, { 0,1,0,1 });
}

void SummonEnemy::DrawDebugGUI(int n)
{
    Character::DrawDebugGUI("SummonEnemy", n);
}

void SummonEnemy::UpdateAttackState(float elapsed_time)
{
}

void SummonEnemy::UpdateMoveState(float elapsed_time)
{
    timer += elapsed_time;
    //TODO もね　攻撃間隔召喚敵　最初(ランダム確率)
    float interval = 6.5f;

    if (first&& interval<=timer)
    {
        PlayAnimation(Animation::Attack, false);
        Enemy* enemy = nullptr;

        switch (rand() % 3)
        {
        case EnemyType::A:
        {
            enemy = new Enemy_A(false);
            EnemySpawner::SetBasicEnemyStatus(enemy, status[rand() % 4]);//ランダムなステータスをセット

            enemy->SetShaft(rand() % 2);// ランダムな軸に
            enemy->SetPosition(position);// 召喚敵の座標を設定
            enemy->SetModelScale(child_scale);
            enemy->UpdateTransform();

            // リストに追加
            EnemyManager::Instance().Register(enemy);
            break;
        }
        case  EnemyType::B:
        {
            enemy = new Enemy_B(false);
            EnemySpawner::SetBasicEnemyStatus(enemy, status[rand() % 4]);//ランダムなステータスをセット
            enemy->SetMoveTimerMax(mover_timer_max_B);
            enemy->SetDis(dis_B);

            enemy->SetShaft(rand() % 2);// ランダムな軸に
            enemy->SetPosition(position);// 召喚敵の座標を設定
            enemy->SetModelScale(child_scale);
            enemy->UpdateTransform();

            // リストに追加
            EnemyManager::Instance().Register(enemy);
            break;
        }
        case   EnemyType::C:
        {
            enemy = new Enemy_C(false);
            EnemySpawner::SetBasicEnemyStatus(enemy, status[rand() % 4]);//ランダムなステータスをセット

            enemy->SetShaft(rand() % 2);// ランダムな軸に
            enemy->SetPosition(position);// 召喚敵の座標を設定
            enemy->SetModelScale(child_scale);
            enemy->UpdateTransform();

            // リストに追加
            EnemyManager::Instance().Register(enemy);
        }
        };

        timer = 0.0f;
        first = false;
    }

    // タイマーが既定の時間になったら敵を召喚
    if (attack_interval < timer )
    {
        PlayAnimation(Animation::Attack, false);
        Enemy* enemy = nullptr;


        switch (rand() % 3)
        {
        case EnemyType::A:
        {
            enemy = new Enemy_A(false);
            EnemySpawner::SetBasicEnemyStatus(enemy, status[rand() % 4]);//ランダムなステータスをセット

            enemy->SetShaft(rand() % 2);// ランダムな軸に
            enemy->SetPosition(position);// 召喚敵の座標を設定
            enemy->SetModelScale(child_scale);
            enemy->UpdateTransform();

            // リストに追加
            EnemyManager::Instance().Register(enemy);
            break;
        }
        case  EnemyType::B:
        {
            enemy = new Enemy_B(false);
            EnemySpawner::SetBasicEnemyStatus(enemy, status[rand() % 4]);//ランダムなステータスをセット
            enemy->SetMoveTimerMax(mover_timer_max_B);
            enemy->SetDis(dis_B);

            enemy->SetShaft(rand() % 2);// ランダムな軸に
            enemy->SetPosition(position);// 召喚敵の座標を設定
            enemy->SetModelScale(child_scale);
            enemy->UpdateTransform();

            // リストに追加
            EnemyManager::Instance().Register(enemy);
            break;
        }
        case   EnemyType::C:
        {
            enemy = new Enemy_C(false);
            EnemySpawner::SetBasicEnemyStatus(enemy, status[rand() % 4]);//ランダムなステータスをセット

            enemy->SetShaft(rand() % 2);// ランダムな軸に
            enemy->SetPosition(position);// 召喚敵の座標を設定
            enemy->SetModelScale(child_scale);
            enemy->UpdateTransform();

            // リストに追加
            EnemyManager::Instance().Register(enemy);
        }
        };

        timer = 0.0f;
    }

    // 死亡ステートへ
    if (IsDead())
    {
        TransitionDeathState();
    }
}



BossEnemy::BossEnemy(bool is_minor)
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    if (!is_minor)LoadFBXModel(graphics.GetDevice(), ".\\resources\\Model\\Enemy\\Bear.fbx");
    else LoadFBXModel(graphics.GetDevice(), ".\\resources\\Model\\Enemy\\Bear2.fbx");

    attack_effect = new Effect(".\\resources\\Effect\\fence_break\\fence_break.efk");
    death_effect = new Effect(".\\resources\\Effect\\ENEMY_DOWN\\ENEMY_DOWN.efk");
    hit_effect = new Effect(".\\resources\\Effect\\hit_to_enemy\\hit_to_enemy.efk");

    attack_power = 1;        // 攻撃力
    attack_interval = 3.0f;     // 攻撃間隔

    health = 10;       // HP
    radius = 1.0f;     // 半径
    height = 1.0f;     // デバッグ用
    speed_power = -1.0f;    // 速度

    // TODO もね　モデル大きさ
    model_scale = 2.0f;

    // とりあえずアニメーション
    PlayAnimation(Animation::Move, true);
}

void BossEnemy::DrawDebugPrimitive()
{
    DebugRenderer* debug_renderer = Lemur::Graphics::Graphics::Instance().GetDebugRenderer();
    //debug_renderer->DrawCylinder(position, radius, height, { 0,1,0,1 });
}
void BossEnemy::DrawDebugGUI(int n)
{
    Character::DrawDebugGUI("BossEnemy", n);
}

void BossEnemy::UpdateAttackState(float elapsed_time)
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
            PlayAnimation(Animation::Attack, false);
            attack_handle = attack_effect->Play(position, attack_effect_size);
            // タイマーをに
            timer = 0.0f;
        }
    }


    // 死亡時は死亡ステートへ切り替え
    if (IsDead())
    {
        TransitionDeathState();
    }
}

void BossEnemy::UpdateMoveState(float elapsed_time)
{
    if (shaft == Shaft::Side) rotation.y = DirectX::XMConvertToRadians(-90);
    else if (shaft == Shaft::Vertical)rotation.y = DirectX::XMConvertToRadians(180);

    // 柵に触れれば攻撃ステートへ
    if (Collision::IntersectRectVsCircle(Fence::Instance().GetLeftRect(), { position.x,position.z }, radius) ||
        Collision::IntersectRectVsCircle(Fence::Instance().GetFrontRect(), { position.x,position.z }, radius))
    {
        TransitionAttackState();
    }
    else
    {
        // 移動
        if (shaft == Shaft::Side) velocity.x = speed_power;
        else if (shaft == Shaft::Vertical)velocity.z = speed_power;
    }

    // 死亡ステートへ
    if (IsDead())
    {
        TransitionDeathState();
    }
}



NuisanceEnemy::NuisanceEnemy(bool is_minor)
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    if (!is_minor)LoadFBXModel(graphics.GetDevice(), ".\\resources\\Model\\Enemy\\Deer.fbx");
    else LoadFBXModel(graphics.GetDevice(), ".\\resources\\Model\\Enemy\\Deer2.fbx");

    attack_effect = new Effect(".\\resources\\Effect\\fence_break\\fence_break.efk");
    death_effect = new Effect(".\\resources\\Effect\\ENEMY_DOWN\\ENEMY_DOWN.efk");
    hit_effect = new Effect(".\\resources\\Effect\\hit_to_enemy\\hit_to_enemy.efk");

    health = 10;            // HP
    radius = 1.0f;          // 半径
    height = 1.0f;          // デバッグ用

    // TODO もね　動かない敵大きさ
     model_scale = 1.3f;

    // とりあえずアニメーション
    PlayAnimation(Animation::Move, true);
}

void NuisanceEnemy::DrawDebugPrimitive()
{
    DebugRenderer* debug_renderer = Lemur::Graphics::Graphics::Instance().GetDebugRenderer();
   // debug_renderer->DrawCylinder(position, radius, height, { 0,1,0,1 });
}

void NuisanceEnemy::DrawDebugGUI(int n)
{
    Character::DrawDebugGUI("NuisanceEnemy", n);
}

void NuisanceEnemy::UpdateMoveState(float elapsed_time)
{
    // 死亡ステートへ
    if (IsDead())
    {
        TransitionDeathState();
    }
}
