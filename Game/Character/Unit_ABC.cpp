#include "Unit_ABC.h"
#include "EnemyManager.h"

//--------Unit_A--------------------------------------------------
Unit_A::Unit_A()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Unit\\Chili_24_0305_01.fbx");

    attack_times = 5;    // 攻撃回数
    attack_power = 1;    // 攻撃力
    attack_interval = 0.5f; // 攻撃間隔
    attack_radius = 1.0f; // 攻撃範囲
    radius = 0.3f; // 半径
    height = 0.5f; // デバッグ用
    dec_pos = 1.0f; // ユニットに接触した種がどのくらい跳ね返されるか

    // とりあえずアニメーション
    model->PlayAnimation(Animation::Idle, true);
}

void Unit_A::DrawDebugPrimitive()
{
    DebugRenderer* debug_renderer = Lemur::Graphics::Graphics::Instance().GetDebugRenderer();
    debug_renderer->DrawCylinder(position, attack_radius, height, { 1,0,0,1 });
    debug_renderer->DrawCylinder(position, radius, height, { 0,1,0,1 });
}

void Unit_A::UpdateIdleState(float elapsed_time)
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
            { position.x,position.z },                // ユニットの位置(XZ平面)
            attack_radius,                          // 攻撃範囲
            { enemy->GetPosition().x,enemy->GetPosition().z },  // 敵の位置(XZ平面)
            enemy->GetRadius()                           // 敵の当たり判定
        ))
        {
            attack_enemy = enemy;
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

void Unit_A::UpdateAttackState(float elapsed_time)
{
    attack_timer += elapsed_time;// 攻撃タイマー

    if (attack_timer >= attack_interval)
    {
        if (attack_enemy->ApplyDamage(attack_power))
        {
            attack_timer = 0.0f;
            attack_times--;
        }
    }


    EnemyManager& enemyManager = EnemyManager::Instance();
    int enemyCount = enemyManager.GetEnemyCount();
    bool is_intersected = false;

    // 敵の総当たり
    for (int j = 0; j < enemyCount; ++j)
    {
        Enemy* enemy = enemyManager.GetEnemy(j);

        // 敵がユニットの攻撃範囲に入っているとき
        if (Collision::IntersectCircleVsCircle
        (
            { position.x,position.z },                // ユニットの位置(XZ平面)
            attack_radius,                          // 攻撃範囲
            { enemy->GetPosition().x,enemy->GetPosition().z },  // 敵の位置(XZ平面)
            enemy->GetRadius()                           // 敵の当たり判定
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

void Unit_A::DrawDebugGUI(int n)
{
    Character::DrawDebugGUI("Unit_A", n);
}


//--------Unit_B--------------------------------------------------

Unit_B::Unit_B()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Unit\\Chili_24_0305_01.fbx");

    attack_times = 5;    // 攻撃回数
    attack_power = 1;    // 攻撃力
    attack_interval = 0.5f; // 攻撃間隔
    attack_radius = 1.0f; // 攻撃範囲
    radius = 0.3f; // 半径
    height = 0.5f; // デバッグ用
    dec_pos = 1.0f; // ユニットに接触した種がどのくらい跳ね返されるか

    // 三角
    t_height = 1.0f;// 高さ
    t_base = 1.0f;// 底辺

    // 頂点をユニットのポジションに
    triangle_1.A = triangle_2.A = { position.x,position.z };
    // 左奥側の頂点
    triangle_1.B = { triangle_1.A.x - t_height,triangle_1.A.y + (t_base * 0.5f) };
    triangle_2.B = { triangle_2.A.x + t_height,triangle_2.A.y + (t_base * 0.5f) };
    // 右手前側の頂点
    triangle_1.C = { triangle_1.A.x - t_height,triangle_1.A.y - (t_base * 0.5f) };
    triangle_2.C = { triangle_2.A.x + t_height,triangle_2.A.y - (t_base * 0.5f) };


    // とりあえずアニメーション
    model->PlayAnimation(Animation::Idle, true);
}

void Unit_B::DrawDebugPrimitive()
{
    DebugRenderer* debug_renderer = Lemur::Graphics::Graphics::Instance().GetDebugRenderer();
    debug_renderer->DrawCylinder(position, radius, height, { 0,1,0,1 });

    // 左三角
    debug_renderer->DrawSphere({ triangle_1.A.x,0.2f,triangle_1.A.y }, 0.1f, { 1,0,1,1 });
    debug_renderer->DrawSphere({ triangle_1.B.x,0.2f,triangle_1.B.y }, 0.1f, { 1,0,1,1 });
    debug_renderer->DrawSphere({ triangle_1.C.x,0.2f,triangle_1.C.y }, 0.1f, { 1,0,1,1 });

    // 右三角
    debug_renderer->DrawSphere({ triangle_2.A.x,0.2f,triangle_2.A.y }, 0.1f, { 0,0,1,1 });
    debug_renderer->DrawSphere({ triangle_2.B.x,0.2f,triangle_2.B.y }, 0.1f, { 0,0,1,1 });
    debug_renderer->DrawSphere({ triangle_2.C.x,0.2f,triangle_2.C.y }, 0.1f, { 0,0,1,1 });
}

void Unit_B::Update(float elapsed_time)
{
    // 頂点をユニットのポジションに
    triangle_1.A = triangle_2.A = { position.x,position.z };
    // 左奥側の頂点
    triangle_1.B = { triangle_1.A.x - t_height,triangle_1.A.y + (t_base * 0.5f) };
    triangle_2.B = { triangle_2.A.x + t_height,triangle_2.A.y + (t_base * 0.5f) };
    // 右手前側の頂点
    triangle_1.C = { triangle_1.A.x - t_height,triangle_1.A.y - (t_base * 0.5f) };
    triangle_2.C = { triangle_2.A.x + t_height,triangle_2.A.y - (t_base * 0.5f) };


    Unit::Update(elapsed_time);
}

void Unit_B::UpdateIdleState(float elapsed_time)
{
    EnemyManager& enemyManager = EnemyManager::Instance();

    int enemyCount = enemyManager.GetEnemyCount();

    //TODO モーションが来たらまた変える
    // 敵の総当たり
    for (int j = 0; j < enemyCount; ++j)
    {
        Enemy* enemy = enemyManager.GetEnemy(j);

        // 敵がユニットの攻撃範囲に入っているとき
        // 左三角
        if (Collision::IntersectTriangleVsCircle
        (
            triangle_1,
            { enemy->GetPosition().x,enemy->GetPosition().z },  // 敵の位置(XZ平面)
            enemy->GetRadius()                           // 敵の当たり判定
        ))
        {
            attack_enemy = enemy;
            TransitionAttackState();
        }
        // 右三角
        if (Collision::IntersectTriangleVsCircle
        (
            triangle_2,
            { enemy->GetPosition().x,enemy->GetPosition().z },  // 敵の位置(XZ平面)
            enemy->GetRadius()                           // 敵の当たり判定
        ))
        {
            attack_enemy = enemy;
            TransitionAttackState();
        }
    }

    // 攻撃回数を消費しきったら消滅
    if (attack_times <= 0)
    {
        TransitionIdleState();
    }
}

void Unit_B::UpdateAttackState(float elapsed_time)
{
    // 攻撃タイマー
    attack_timer += elapsed_time;

    if (attack_timer >= attack_interval)
    {
        if (attack_enemy->ApplyDamage(attack_power))
        {
            attack_timer = 0.0f;// タイマーを初期化
            attack_times--;// 攻撃回数を減らす
        }
    }


    EnemyManager& enemyManager = EnemyManager::Instance();
    int enemyCount = enemyManager.GetEnemyCount();
    bool is_intersected = false;

    // 敵の総当たり
    for (int j = 0; j < enemyCount; ++j)
    {
        Enemy* enemy = enemyManager.GetEnemy(j);

        // 敵がユニットの攻撃範囲に入っているとき
        // 左三角
        if (Collision::IntersectTriangleVsCircle
        (
            triangle_1,
            { enemy->GetPosition().x,enemy->GetPosition().z },  // 敵の位置(XZ平面)
            enemy->GetRadius()                           // 敵の当たり判定
        ))
        {
            is_intersected = true;
            break;
        }
        // 右三角
        if (Collision::IntersectTriangleVsCircle
        (
            triangle_2,
            { enemy->GetPosition().x,enemy->GetPosition().z },  // 敵の位置(XZ平面)
            enemy->GetRadius()                           // 敵の当たり判定
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

void Unit_B::DrawDebugGUI(int n)
{
    Character::DrawDebugGUI("Unit_B", n);
}


//--------Unit_C--------------------------------------------------

Unit_C::Unit_C()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Unit\\Chili_24_0305_01.fbx");

    attack_times = 5;    // 攻撃回数
    attack_power = 1;    // 攻撃力
    attack_interval = 0.5f; // 攻撃間隔
    attack_radius = 1.0f; // 攻撃範囲
    radius = 0.3f; // 半径
    height = 0.5f; // デバッグ用
    dec_pos = 1.0f; // ユニットに接触した種がどのくらい跳ね返されるか

    // 三角
    t_height = 1.0f;// 高さ
    t_base = 1.0f;// 底辺

    // triangle_1   =奥、triangle_2=手前
    // 頂点をユニットのポジションに
    triangle_1.A = triangle_2.A = { position.x,position.z };
    // 左側の頂点
    triangle_1.B = { triangle_1.A.x - (t_base * 0.5f),triangle_1.A.y + t_height };
    triangle_2.B = { triangle_2.A.x - (t_base * 0.5f),triangle_2.A.y - t_height };
    // 右側の頂点
    triangle_1.C = { triangle_1.A.x + (t_base * 0.5f),triangle_1.A.y + t_height };
    triangle_2.C = { triangle_2.A.x + (t_base * 0.5f),triangle_2.A.y - t_height };


    // とりあえずアニメーション
    model->PlayAnimation(Animation::Idle, true);
}

void Unit_C::DrawDebugPrimitive()
{
    DebugRenderer* debug_renderer = Lemur::Graphics::Graphics::Instance().GetDebugRenderer();
    debug_renderer->DrawCylinder(position, radius, height, { 0,1,0,1 });

    // 奥三角
    debug_renderer->DrawSphere({ triangle_1.A.x,0.2f,triangle_1.A.y }, 0.1f, { 1,0,1,1 });
    debug_renderer->DrawSphere({ triangle_1.B.x,0.2f,triangle_1.B.y }, 0.1f, { 1,0,1,1 });
    debug_renderer->DrawSphere({ triangle_1.C.x,0.2f,triangle_1.C.y }, 0.1f, { 1,0,1,1 });

    // 手前三角
    debug_renderer->DrawSphere({ triangle_2.A.x,0.2f,triangle_2.A.y }, 0.1f, { 0,0,1,1 });
    debug_renderer->DrawSphere({ triangle_2.B.x,0.2f,triangle_2.B.y }, 0.1f, { 0,0,1,1 });
    debug_renderer->DrawSphere({ triangle_2.C.x,0.2f,triangle_2.C.y }, 0.1f, { 0,0,1,1 });
}

void Unit_C::Update(float elapsed_time)
{
    // triangle_1   =奥、triangle_2=手前
    // 頂点をユニットのポジションに
    triangle_1.A = triangle_2.A = { position.x,position.z };
    // 左側の頂点
    triangle_1.B = { triangle_1.A.x - (t_base * 0.5f),triangle_1.A.y + t_height };
    triangle_2.B = { triangle_2.A.x - (t_base * 0.5f),triangle_2.A.y - t_height };
    // 右側の頂点
    triangle_1.C = { triangle_1.A.x + (t_base * 0.5f),triangle_1.A.y + t_height };
    triangle_2.C = { triangle_2.A.x + (t_base * 0.5f),triangle_2.A.y - t_height };

    Unit::Update(elapsed_time);
}

void Unit_C::UpdateIdleState(float elapsed_time)
{
    EnemyManager& enemyManager = EnemyManager::Instance();

    int enemyCount = enemyManager.GetEnemyCount();

    //TODO モーションが来たらまた変える
    // 敵の総当たり
    for (int j = 0; j < enemyCount; ++j)
    {
        Enemy* enemy = enemyManager.GetEnemy(j);

        // 敵がユニットの攻撃範囲に入っているとき
        // 奥三角
        if (Collision::IntersectTriangleVsCircle
        (
            triangle_1,
            { enemy->GetPosition().x,enemy->GetPosition().z },  // 敵の位置(XZ平面)
            enemy->GetRadius()                           // 敵の当たり判定
        ))
        {
            attack_enemy = enemy;
            TransitionAttackState();
        }
        // 手前三角
        if (Collision::IntersectTriangleVsCircle
        (
            triangle_2,
            { enemy->GetPosition().x,enemy->GetPosition().z },  // 敵の位置(XZ平面)
            enemy->GetRadius()                           // 敵の当たり判定
        ))
        {
            attack_enemy = enemy;
            TransitionAttackState();
        }
    }

    // 攻撃回数を消費しきったら消滅
    if (attack_times <= 0)
    {
        TransitionIdleState();
    }
}

void Unit_C::UpdateAttackState(float elapsed_time)
{
    // 攻撃タイマー
    attack_timer += elapsed_time;

    if (attack_timer >= attack_interval)
    {
        if (attack_enemy->ApplyDamage(attack_power))
        {
            attack_timer = 0.0f;// タイマーを初期化
            attack_times--;// 攻撃回数を減らす
        }
    }


    EnemyManager& enemyManager = EnemyManager::Instance();
    int enemyCount = enemyManager.GetEnemyCount();
    bool is_intersected = false;

    // 敵の総当たり
    for (int j = 0; j < enemyCount; ++j)
    {
        Enemy* enemy = enemyManager.GetEnemy(j);

        // 敵がユニットの攻撃範囲に入っているとき
        // 奥三角
        if (Collision::IntersectTriangleVsCircle
        (
            triangle_1,
            { enemy->GetPosition().x,enemy->GetPosition().z },  // 敵の位置(XZ平面)
            enemy->GetRadius()                           // 敵の当たり判定
        ))
        {
            is_intersected = true;
            break;
        }
        // 手前三角
        if (Collision::IntersectTriangleVsCircle
        (
            triangle_2,
            { enemy->GetPosition().x,enemy->GetPosition().z },  // 敵の位置(XZ平面)
            enemy->GetRadius()                           // 敵の当たり判定
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
void Unit_C::DrawDebugGUI(int n)
{
    Character::DrawDebugGUI("Unit_C", n);
}
