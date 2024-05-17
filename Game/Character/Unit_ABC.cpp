#include "Unit_ABC.h"
#include "EnemyManager.h"
#include "UnitManager.h"

//--------Chili--------------------------------------------------
Chili::Chili()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    //LoadGltfModel(graphics.GetDevice(), ".\\resources\\Model_glb\\Unit\\Chili.glb",true);
    LoadFBXModel(graphics.GetDevice(), ".\\resources\\Model\\Unit\\Chili.fbx");

    collision_model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Collision\\circle.fbx");


    attack_effect = new Effect(".\\resources\\Effect\\UNIT1\\UNIT1.efk");
    death_effect = new Effect(".\\resources\\Effect\\UNIT_DEATH\\UNIT_DEATH.efk");
    set_effect = new Effect(".\\resources\\Effect\\UNIT_SET\\UNIT_SET.efk");

    attack_times           = 5;    // 攻撃回数
    attack_power           = 1;    // 攻撃力
    attack_interval        = 0.1f; // 攻撃間隔
    attack_collision_range = 1.0f; // 攻撃範囲
    height                 = 0.5f; // デバッグ用
    dec_pos                = 1.0f; // ユニットに接触した種がどのくらい跳ね返されるか

    radius                 = 0.4f; // 半径

    attack_effect_size = 0.5f;
    set_effect_size = 0.5f;
    death_effect_size = 0.5f;

    UpdateTransform();

    // とりあえずアニメーション
    PlayAnimation(Animation::Idle, true);
}

void Chili::DrawDebugPrimitive()
{
    //DebugRenderer* debug_renderer = Lemur::Graphics::Graphics::Instance().GetDebugRenderer();
    //debug_renderer->DrawCylinder(position, attack_collision_range, height, { 1,0,0,1 });
    //debug_renderer->DrawCylinder(position, radius, height, { 0,1,0,1 });
}

void Chili::UpdateIdleState(float elapsed_time)
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
        if (Collision::IntersectCircleVsCircle
        (
            { position.x,position.z },                          // ユニットの位置(XZ平面)
            attack_collision_range,                             // 攻撃範囲
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

void Chili::UpdateAttackState(float elapsed_time)
{
    EnemyManager& enemyManager = EnemyManager::Instance();
    int enemyCount = enemyManager.GetEnemyCount();

    // 敵が範囲内にいるかどうか
    bool is_intersected = false;

    attack_timer += elapsed_time;// 攻撃間隔用タイマー

    // ２撃目以降はタイマーが規定時間を超えたら攻撃
    if (attack_timer >= attack_interval)is_attack = true;

    if (attack_times > 0)// 攻撃回数が残っているとき
    {
        // 敵の総当たり
        for (int j = 0; j < enemyCount; ++j)
        {
            Enemy* enemy = enemyManager.GetEnemy(j);

            // 敵が死んでたらコンティニュー
            if (enemy->IsDead())continue;

            // ユニットの攻撃範囲に入っている敵全員に攻撃
            if (Collision::IntersectCircleVsCircle
            (
                { position.x,position.z },                          // ユニットの位置(XZ平面)
                attack_collision_range,                             // 攻撃範囲
                { enemy->GetPosition().x,enemy->GetPosition().z },  // 敵の位置(XZ平面)
                enemy->GetRadius()                                  // 敵の当たり判定
            ))
            {
                // 敵とかぶったフラグをON
                is_intersected = true;
                if (is_attack)
                {
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
        if(!IsPlayAnimation()) TransitionDeathState();
    }

}

void Chili::DrawDebugGUI(int n)
{
    Character::DrawDebugGUI("Chili", n);
}


//--------Shishito--------------------------------------------------

Shishito::Shishito()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    //LoadGltfModel(graphics.GetDevice(), ".\\resources\\Model_glb\\Unit\\Shishito.glb",true);
    LoadFBXModel(graphics.GetDevice(), ".\\resources\\Model\\Unit\\Shishito.fbx");

    collision_model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Collision\\circle.fbx");
    collision_scale_facter = 2.0f;

    attack_effect = new Effect(".\\resources\\Effect\\UNIT4_BUFF\\UNIT4_BUFF.efk");
    death_effect = new Effect(".\\resources\\Effect\\UNIT_DEATH\\UNIT_DEATH.efk");
    set_effect = new Effect(".\\resources\\Effect\\UNIT_SET\\UNIT_SET.efk");

    attack_times = 5;    // 攻撃回数
    attack_power = 1;    // 攻撃力
    streng_width = 2;    // 強化幅
    attack_interval = 0.5f; // 攻撃間隔
    attack_collision_range = 1.0f; // 攻撃範囲
    height = 0.5f; // デバッグ用
    dec_pos = 1.0f; // ユニットに接触した種がどのくらい跳ね返されるか
    timer_max = 3.0f; // バフの秒数
    radius = 0.4f; // 半径

    // とりあえずアニメーション
    PlayAnimation(Animation::Idle, true);
}

void Shishito::DrawDebugPrimitive()
{
   //DebugRenderer* debug_renderer = Lemur::Graphics::Graphics::Instance().GetDebugRenderer();
   //debug_renderer->DrawCylinder(position, attack_collision_range, height, { 1,0,0,1 });
   //debug_renderer->DrawCylinder(position, radius, height, { 0,1,0,1 });
}

void Shishito::UpdateIdleState(float elapsed_time)
{
    UnitManager& unitManager = UnitManager::Instance();
    int unitCount = unitManager.GetUnitCount();

    // 攻撃ステートに切り替え
    PlayAnimation(Animation::Attack, true);
    TransitionAttackState();

    attack_timer += elapsed_time;// 攻撃タイマー

    // 強制的にアタックに
    TransitionAttackState();

    //TODO モーションが来たらまた変える
    // ユニットの総当たり
    for (int j = 0; j < unitCount; ++j)
    {
        Unit* unit = unitManager.GetUnit(j);
        if (unit->GetCategory() == UnitManager::UNIT_INDEX::Shishito)continue;
        // ユニットが死んでたらコンティニュー
        if (unit->IsDead())continue;

        // 他のユニットがユニットの攻撃範囲に入っているとき
        if (Collision::IntersectCircleVsCircle
        (
            { position.x,position.z },                        // ユニットの位置(XZ平面)
            attack_collision_range,                           // 攻撃範囲
            { unit->GetPosition().x,unit->GetPosition().z },  // 敵の位置(XZ平面)
            unit->GetRadius()                                 // 敵の当たり判定
        ))
        {
            // 強化状態をtrueに
            unit->SetBuff(true);
            unit->SetStrengAttack(unit->GetAttackPower() + streng_width);

            // アニメーションとエフェクト
            attack_handle = attack_effect->Play(position, attack_effect_size);

            // 攻撃ステートに切り替え
            PlayAnimation(Animation::Attack, false);
            TransitionAttackState();
        }
    }
    // 攻撃回数を消費しきったら消滅
    if (attack_timer >= timer_max)
    {
        TransitionDeathState();
    }
}

void Shishito::UpdateAttackState(float elapsed_time)
{
    attack_timer += elapsed_time;// 攻撃タイマー

    UnitManager& unitManager = UnitManager::Instance();
    int unitCount = unitManager.GetUnitCount();
    bool is_intersected = false;

    //TODO モーションが来たらまた変える
    // ユニットの総当たり
    for (int j = 0; j < unitCount; ++j)
    {
        Unit* unit = unitManager.GetUnit(j);

        if (unit->GetCategory() == UnitManager::UNIT_INDEX::Shishito)continue;
        // ユニットが死んでたらコンティニュー
        if (unit->IsDead())continue;

        // 他のユニットがユニットの攻撃範囲に入っているとき
        if (Collision::IntersectCircleVsCircle
        (
            { position.x,position.z },                        // ユニットの位置(XZ平面)
            attack_collision_range,                           // 攻撃範囲
            { unit->GetPosition().x,unit->GetPosition().z },  // 敵の位置(XZ平面)
            unit->GetRadius()                                 // 敵の当たり判定
        ))
        {
            is_intersected = true;
            // 強化状態をtrueに
            unit->SetBuff(true);
            unit->SetStrengAttack(unit->GetAttackPower() + streng_width);
            if (!IsPlayAnimation())
            {
                PlayAnimation(Animation::Attack, false);
                attack_handle = attack_effect->Play(position, attack_effect_size);
            }
        }
        else// 範囲内にいない
        {
            // 強化状態を切る
            unit->SetBuff(true);
        }
    }

    //// 範囲内に敵が一体も居なければ待機
    //if (!is_intersected)
    //{
    //    TransitionIdleState();
    //}

    // 攻撃時間が過ぎたら消滅
    if (attack_timer >= timer_max)
    {
        attack_effect->Stop(attack_handle);
        TransitionDeathState();
    }
}

void Shishito::DrawDebugGUI(int n)
{
    Character::DrawDebugGUI("Shishito", n);
}


//--------OrangePumpkin--------------------------------------------------

OrangePumpkin::OrangePumpkin()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    //LoadGltfModel(graphics.GetDevice(), ".\\resources\\Model_glb\\Unit\\OrangePumpkin.glb",true);
    LoadFBXModel(graphics.GetDevice(), ".\\resources\\Model\\Unit\\OrangePumpkin.fbx");

    collision_model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Collision\\triangle_collision.fbx");

    attack_effect = new Effect(".\\resources\\Effect\\UNIT2_ATK\\UNIT2_ATK.efk");
    death_effect = new Effect(".\\resources\\Effect\\UNIT_DEATH\\UNIT_DEATH.efk");
    set_effect = new Effect(".\\resources\\Effect\\UNIT_SET\\UNIT_SET.efk");

    attack_times = 5;    // 攻撃回数
    attack_power = 1;    // 攻撃力
    attack_interval = 0.5f; // 攻撃間隔
    attack_collision_range = 1.0f; // 攻撃範囲
    radius = 0.4f; // 半径
    height = 0.5f; // デバッグ用
    dec_pos = 1.0f; // ユニットに接触した種がどのくらい跳ね返されるか

    // 三角
    t_height               = 1.0f;// 高さ
    t_base                 = 1.0f;// 底辺

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

void OrangePumpkin::DrawDebugPrimitive()
{
    //DebugRenderer* debug_renderer = Lemur::Graphics::Graphics::Instance().GetDebugRenderer();
    //debug_renderer->DrawCylinder(position, radius, height, { 0,1,0,1 });

    //// 奥三角
    //debug_renderer->DrawSphere({ triangle_1.A.x,0.2f,triangle_1.A.y }, 0.1f, { 1,0,1,1 });
    //debug_renderer->DrawSphere({ triangle_1.B.x,0.2f,triangle_1.B.y }, 0.1f, { 1,0,1,1 });
    //debug_renderer->DrawSphere({ triangle_1.C.x,0.2f,triangle_1.C.y }, 0.1f, { 1,0,1,1 });

    //// 手前三角
    //debug_renderer->DrawSphere({ triangle_2.A.x,0.2f,triangle_2.A.y }, 0.1f, { 0,0,1,1 });
    //debug_renderer->DrawSphere({ triangle_2.B.x,0.2f,triangle_2.B.y }, 0.1f, { 0,0,1,1 });
    //debug_renderer->DrawSphere({ triangle_2.C.x,0.2f,triangle_2.C.y }, 0.1f, { 0,0,1,1 });
}

void OrangePumpkin::Update(float elapsed_time)
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

void OrangePumpkin::UpdateIdleState(float elapsed_time)
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

void OrangePumpkin::UpdateAttackState(float elapsed_time)
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
                    // エフェクトの再生
                    attack_handle = attack_effect->Play(position, attack_effect_size);
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
void OrangePumpkin::DrawDebugGUI(int n)
{
    Character::DrawDebugGUI("OrangePumpkin", n);
}
