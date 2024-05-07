#include "Unit_DEF.h"
#include "UnitManager.h"
#include "EnemyManager.h"
#include "Game/Stage/StageManager.h"

//--------Unit_D--------------------------------------------------

Unit_D::Unit_D()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Unit\\unit4_RE.fbx");

    attack_times            = 5;    // 攻撃回数
    attack_power            = 1;    // 攻撃力
    streng_width            = 2;    // 強化幅
    attack_interval         = 0.5f; // 攻撃間隔
    attack_collision_range  = 1.0f; // 攻撃範囲
    radius                  = 0.4f; // 半径
    height                  = 0.5f; // デバッグ用
    dec_pos                 = 1.0f; // ユニットに接触した種がどのくらい跳ね返されるか
    timer_max               = 1.0f; // バフの秒数

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

    attack_timer += elapsed_time;// 攻撃タイマー

    //TODO モーションが来たらまた変える
    // ユニットの総当たり
    for (int j = 0; j < unitCount; ++j)
    {
        Unit* unit = unitManager.GetUnit(j);

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
        }
        else// 範囲内にいない敵は
        {
            // 強化状態を切る
            unit->SetBuff(true);
        }
    }

    // 範囲内に敵が一体も居なければ待機
    if (!is_intersected)
    {
        TransitionIdleState();
    }

    // 攻撃時間が過ぎたら消滅
    if (attack_timer >= timer_max)
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
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Unit\\unit5_RE.fbx");

    attack_times = 5;    // 攻撃回数
    attack_power = 1;    // 攻撃力
    attack_width = 1.0f;    // 攻撃幅
    attack_interval = 0.5f; // 攻撃間隔
    attack_collision_range = 1.0f; // 攻撃範囲
    radius = 0.4f; // 半径
    height = 0.5f; // デバッグ用
    dec_pos = 1.0f; // ユニットに接触した種がどのくらい跳ね返されるか
    Rect stage_size = StageManager::Instance().GetStage(0)->GetStageCollision();
    attack_rect =
    {
        {stage_size.left_up.x,position.z + attack_width},
        {stage_size.right_down.x,position.z - attack_width},
    };

    // とりあえずアニメーション
    model->PlayAnimation(Animation::Idle, true);
}

void Unit_E::DrawDebugPrimitive()
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

void Unit_E::Update(float elapsed_time)
{
    Rect stage_size = StageManager::Instance().GetStage(0)->GetStageCollision();
    attack_rect =
    {
        {stage_size.left_up.x,position.z + attack_width},
        {stage_size.right_down.x,position.z - attack_width},
    };

    Unit::Update(elapsed_time);
}

void Unit_E::UpdateIdleState(float elapsed_time)
{
    EnemyManager& enemyManager = EnemyManager::Instance();
    int enemyCount = enemyManager.GetEnemyCount();

    //TODO モーションが来たらまた変える
    // 敵の総当たり
    for (int j = 0; j < enemyCount; ++j)
    {
        Enemy* enemy = enemyManager.GetEnemy(j);

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

void Unit_E::UpdateAttackState(float elapsed_time)
{
    EnemyManager& enemyManager = EnemyManager::Instance();
    int enemyCount = enemyManager.GetEnemyCount();
    bool is_intersected = false;

    attack_timer += elapsed_time;// 攻撃タイマー
    // タイマーが規定時間を超えたら攻撃
    if (attack_timer >= attack_interval)is_attack = true;

    //TODO モーションが来たらまた変える
    // 敵の総当たり
    for (int j = 0; j < enemyCount; ++j)
    {
        Enemy* enemy = enemyManager.GetEnemy(j);

        // ユニットの攻撃範囲に入っている敵全員に攻撃
        if (Collision::IntersectRectVsCircle
        (
            attack_rect,                                        // 攻撃範囲
            { enemy->GetPosition().x,enemy->GetPosition().z },  // 敵の位置(XZ平面)
            enemy->GetRadius()                                  // 敵の当たり判定
        ))
        {
            int damage = ReturnDamage();
            is_intersected = true;
            if (is_attack)  enemy->ApplyDamage(damage);
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

    // 攻撃回数を消費しきったら消滅
    if (attack_times <= 0)
    {
        TransitionDeathState();
    }
}

void Unit_E::DrawDebugGUI(int n)
{
    Character::DrawDebugGUI("Unit_E", n);
}


//--------Unit_F--------------------------------------------------

Unit_F::Unit_F()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Unit\\unit6_RE.fbx");

    attack_times = 5;    // 攻撃回数
    attack_power = 1;    // 攻撃力
    attack_width = 1.0f;    // 攻撃幅
    attack_interval = 0.5f; // 攻撃間隔
    attack_collision_range = 1.0f; // 攻撃範囲
    radius = 0.4f; // 半径
    height = 0.5f; // デバッグ用
    dec_pos = 1.0f; // ユニットに接触した種がどのくらい跳ね返されるか
    Rect stage_size = StageManager::Instance().GetStage(0)->GetStageCollision();
    attack_rect =
    {
        {position.x-attack_width,stage_size.left_up.y},
        {position.x + attack_width,stage_size.right_down.y},
    };

    // とりあえずアニメーション
    model->PlayAnimation(Animation::Idle, true);
}

void Unit_F::DrawDebugPrimitive()
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

void Unit_F::Update(float elapsed_time)
{
    Rect stage_size = StageManager::Instance().GetStage(0)->GetStageCollision();
    attack_rect =
    {
        {position.x - attack_width,stage_size.left_up.y},
        {position.x + attack_width,stage_size.right_down.y},
    };

    Unit::Update(elapsed_time);
}

void Unit_F::UpdateIdleState(float elapsed_time)
{
    EnemyManager& enemyManager = EnemyManager::Instance();
    int enemyCount = enemyManager.GetEnemyCount();

    //TODO モーションが来たらまた変える
    // 敵の総当たり
    for (int j = 0; j < enemyCount; ++j)
    {
        Enemy* enemy = enemyManager.GetEnemy(j);

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

void Unit_F::UpdateAttackState(float elapsed_time)
{
    EnemyManager& enemyManager = EnemyManager::Instance();
    int enemyCount = enemyManager.GetEnemyCount();
    bool is_intersected = false;

    attack_timer += elapsed_time;// 攻撃タイマー
    // タイマーが規定時間を超えたら攻撃
    if (attack_timer >= attack_interval)is_attack = true;

    //TODO モーションが来たらまた変える
    // 敵の総当たり
    for (int j = 0; j < enemyCount; ++j)
    {
        Enemy* enemy = enemyManager.GetEnemy(j);

        // ユニットの攻撃範囲に入っている敵全員に攻撃
        if (Collision::IntersectRectVsCircle
        (
            attack_rect,                                        // 攻撃範囲
            { enemy->GetPosition().x,enemy->GetPosition().z },  // 敵の位置(XZ平面)
            enemy->GetRadius()                                  // 敵の当たり判定
        ))
        {
            int damage = ReturnDamage();
            is_intersected = true;
            if (is_attack)  enemy->ApplyDamage(damage);
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

    // 攻撃回数を消費しきったら消滅
    if (attack_times <= 0)
    {
        TransitionDeathState();
    }
}

void Unit_F::DrawDebugGUI(int n)
{
    Character::DrawDebugGUI("Unit_F", n);
}
