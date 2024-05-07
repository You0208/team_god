#include "Unit_DEF.h"
#include "EnemyManager.h"
#include "Game/Stage/StageManager.h"

//--------GreenPumpkin--------------------------------------------------

GreenPumpkin::GreenPumpkin()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    //LoadGltfModel(graphics.GetDevice(), ".\\resources\\Model_glb\\Unit\\unit2_RE.glb", true);
    LoadFBXModel(graphics.GetDevice(), ".\\resources\\Model\\Unit\\GreenPumpkin.fbx");

    attack_effect = new Effect(".\\resources\\Effect\\UNIT2_ATK\\UNIT2_ATK.efk");
    death_effect = new Effect(".\\resources\\Effect\\UNIT_DEATH\\UNIT_DEATH.efk");
    set_effect = new Effect(".\\resources\\Effect\\UNIT_SET\\UNIT_SET.efk");

    attack_times = 5;    // 攻撃回数
    attack_power = 1;    // 攻撃力
    attack_interval = 0.5f; // 攻撃間隔
    attack_collision_range = 1.0f; // 攻撃範囲
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

    UpdateTransform();


    // とりあえずアニメーション
    PlayAnimation(Animation::Idle, true);
}

void GreenPumpkin::DrawDebugPrimitive()
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

void GreenPumpkin::Update(float elapsed_time)
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

void GreenPumpkin::UpdateIdleState(float elapsed_time)
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
        // 左三角
        if (Collision::IntersectTriangleVsCircle
        (
            triangle_1,
            { enemy->GetPosition().x,enemy->GetPosition().z },  // 敵の位置(XZ平面)
            enemy->GetRadius()                           // 敵の当たり判定
        ))
        {
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
            TransitionAttackState();
        }
    }

    // 攻撃回数を消費しきったら消滅
    if (attack_times <= 0)
    {
        TransitionIdleState();
    }
}

void GreenPumpkin::UpdateAttackState(float elapsed_time)
{
    EnemyManager& enemyManager = EnemyManager::Instance();
    int enemyCount = enemyManager.GetEnemyCount();
    bool is_intersected = false;

    attack_timer += elapsed_time;// 攻撃タイマー
    // タイマーが規定時間を超えたら攻撃
    if (attack_timer >= attack_interval)is_attack = true;

    if (attack_times > 0)// 攻撃回数が残っているとき
    {
        //TODO モーションが来たらまた変える
        // 敵の総当たり
        for (int j = 0; j < enemyCount; ++j)
        {
            Enemy* enemy = enemyManager.GetEnemy(j);

            // ユニットが死んでたらコンティニュー
            if (enemy->IsDead())continue;

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
                if (is_attack)
                {
                    // アニメーションの切り替え
                    PlayAnimation(Animation::Attack, false);
                    // 攻撃フラグがONならダメージ処理
                    enemy->ApplyDamage(ReturnDamage());
                }
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
               
                if (is_attack)
                {
                    // アニメーションの切り替え
                    PlayAnimation(Animation::Attack, false);
                    // 攻撃フラグがONならダメージ処理
                    enemy->ApplyDamage(ReturnDamage());
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

void GreenPumpkin::DrawDebugGUI(int n)
{
    Character::DrawDebugGUI("GreenPumpkin", n);
}


//--------Broccoli--------------------------------------------------

Broccoli::Broccoli()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    // LoadGltfModel(graphics.GetDevice(), ".\\resources\\Model_glb\\Unit\\Broccoli.glb",true);
    LoadFBXModel(graphics.GetDevice(), ".\\resources\\Model\\Unit\\Broccoli.fbx");

    attack_times = 5;    // 攻撃回数
    attack_power = 1;    // 攻撃力
    attack_width = 1.0f;    // 攻撃幅
    attack_interval = 0.5f; // 攻撃間隔
    attack_collision_range = 1.0f; // 攻撃範囲
    radius = 0.3f; // 半径
    height = 0.5f; // デバッグ用
    dec_pos = 1.0f; // ユニットに接触した種がどのくらい跳ね返されるか
    Rect stage_size = StageManager::Instance().GetStage(0)->GetStageCollision();
    attack_rect =
    {
        {stage_size.left_up.x,position.z + attack_width},
        {stage_size.right_down.x,position.z - attack_width},
    };

    // とりあえずアニメーション
    PlayAnimation(Animation::Idle, true);
}

void Broccoli::DrawDebugPrimitive()
{
    DebugRenderer* debug_renderer = Lemur::Graphics::Graphics::Instance().GetDebugRenderer();
    DirectX::XMFLOAT3 attack_rect_size = {
        (attack_rect.right_down.x - attack_rect.left_up.x) * 0.5f,
        0.5f,
        (attack_rect.left_up.y - attack_rect.right_down.y) * 0.5f
    };
    DirectX::XMFLOAT3 attack_rect_center = {
        attack_rect.left_up.x + attack_rect_size.x,
        0,
        attack_rect.right_down.y + attack_rect_size.z
    };

    debug_renderer->DrawBox(attack_rect_center, { 0,0,0 }, attack_rect_size, { 1,0,1,1 });
}

void Broccoli::Update(float elapsed_time)
{
    Rect stage_size = StageManager::Instance().GetStage(0)->GetStageCollision();
    attack_rect =
    {
        {stage_size.left_up.x,position.z + attack_width},
        {stage_size.right_down.x,position.z - attack_width},
    };

    Unit::Update(elapsed_time);
}

void Broccoli::UpdateIdleState(float elapsed_time)
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
        if (Collision::IntersectRectVsCircle
        (
            attack_rect,
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

void Broccoli::UpdateAttackState(float elapsed_time)
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

            // 敵が死んでたらコンティニュー
            if (enemy->IsDead())continue;

            // 敵の攻撃範囲に入っている敵全員に攻撃
            if (Collision::IntersectRectVsCircle
            (
                attack_rect,                                        // 攻撃範囲
                { enemy->GetPosition().x,enemy->GetPosition().z },  // 敵の位置(XZ平面)
                enemy->GetRadius()                                  // 敵の当たり判定
            ))
            {
                is_intersected = true;
                // アニメーションの切り替え
                if (is_attack)
                {
                    PlayAnimation(Animation::Attack, false);
                    enemy->ApplyDamage(ReturnDamage());
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

void Broccoli::DrawDebugGUI(int n)
{
    Character::DrawDebugGUI("Broccoli", n);
}


//--------Cauliflower--------------------------------------------------

Cauliflower::Cauliflower()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    //LoadGltfModel(graphics.GetDevice(), ".\\resources\\Model_glb\\Unit\\Cauliflower.glb",true);
    LoadFBXModel(graphics.GetDevice(), ".\\resources\\Model\\Unit\\Cauliflower.fbx");

    attack_times = 5;    // 攻撃回数
    attack_power = 1;    // 攻撃力
    attack_width = 1.0f;    // 攻撃幅
    attack_interval = 0.5f; // 攻撃間隔
    attack_collision_range = 1.0f; // 攻撃範囲
    radius = 0.3f; // 半径
    height = 0.5f; // デバッグ用
    dec_pos = 1.0f; // ユニットに接触した種がどのくらい跳ね返されるか
    Rect stage_size = StageManager::Instance().GetStage(0)->GetStageCollision();
    attack_rect =
    {
        {position.x-attack_width,stage_size.left_up.y},
        {position.x + attack_width,stage_size.right_down.y},
    };

    // とりあえずアニメーション
    PlayAnimation(Animation::Idle, true);
}

void Cauliflower::DrawDebugPrimitive()
{
    DebugRenderer* debug_renderer = Lemur::Graphics::Graphics::Instance().GetDebugRenderer();
    DirectX::XMFLOAT3 attack_rect_size = {
        (attack_rect.right_down.x - attack_rect.left_up.x) * 0.5f,
        0.5f,
        (attack_rect.left_up.y - attack_rect.right_down.y) * 0.5f
    };
    DirectX::XMFLOAT3 attack_rect_center = {
        attack_rect.left_up.x + attack_rect_size.x,
        0,
        attack_rect.right_down.y + attack_rect_size.z
    };

    debug_renderer->DrawBox(attack_rect_center, { 0,0,0 }, attack_rect_size, { 1,0,1,1 });
}

void Cauliflower::Update(float elapsed_time)
{
    Rect stage_size = StageManager::Instance().GetStage(0)->GetStageCollision();
    attack_rect =
    {
        {position.x - attack_width,stage_size.left_up.y},
        {position.x + attack_width,stage_size.right_down.y},
    };

    Unit::Update(elapsed_time);
}

void Cauliflower::UpdateIdleState(float elapsed_time)
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
        if (Collision::IntersectRectVsCircle
        (
            attack_rect,
            { enemy->GetPosition().x,enemy->GetPosition().z },  // 敵の位置(XZ平面)
            enemy->GetRadius()                                  // 敵の当たり判定
        ))
        {
            // 攻撃ステートに切り替え
            TransitionAttackState();
        }
    }
    // 攻撃回数を消費しきったら消滅
    if (attack_times <= 0 || death)
    {
        TransitionDeathState();
    }
}

void Cauliflower::UpdateAttackState(float elapsed_time)
{
    EnemyManager& enemyManager = EnemyManager::Instance();
    int enemyCount = enemyManager.GetEnemyCount();
    bool is_intersected = false;

    attack_timer += elapsed_time;// 攻撃タイマー
    // タイマーが規定時間を超えたら攻撃
    if (attack_timer >= attack_interval)is_attack = true;

    if (attack_times > 0)// 攻撃回数が残っているとき
    {
        //TODO モーションが来たらまた変える
        // 敵の総当たり
        for (int j = 0; j < enemyCount; ++j)
        {
            Enemy* enemy = enemyManager.GetEnemy(j);

            // ユニットが死んでたらコンティニュー
            if (enemy->IsDead())continue;

            // ユニットの攻撃範囲に入っている敵全員に攻撃
            if (Collision::IntersectRectVsCircle
            (
                attack_rect,                                        // 攻撃範囲
                { enemy->GetPosition().x,enemy->GetPosition().z },  // 敵の位置(XZ平面)
                enemy->GetRadius()                                  // 敵の当たり判定
            ))
            {
                is_intersected = true;
                // アニメーションの切り替え
                if (is_attack)
                {
                    PlayAnimation(Animation::Attack, false);
                    enemy->ApplyDamage(ReturnDamage());
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

void Cauliflower::DrawDebugGUI(int n)
{
    Character::DrawDebugGUI("Cauliflower", n);
}
