#include "Unit_HIJ.h"
#include "EnemyManager.h"


Unit_H::Unit_H()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Unit\\unit3_RE.fbx");

    attack_times = 5;    // 攻撃回数
    attack_power = 1;    // 攻撃力
    attack_interval = 0.5f; // 攻撃間隔
    attack_collision_range = 1.0f; // 攻撃範囲
    radius = 0.3f; // 半径
    height = 0.5f; // デバッグ用
    dec_pos = 1.0f; // ユニットに接触した種がどのくらい跳ね返されるか
    timer_max = 1.0f; // バフの秒数
    rect_angle = 50.0f; // 矩形角度
    attack_rect.center = { position.x ,position.z };
    attack_rect.width = { 1.0f ,20.0f };
    attack_rect = CalcRotateRect(attack_rect.center, attack_rect.width, rect_angle);

    // とりあえずアニメーション
    model->PlayAnimation(Animation::Idle, true);
}

void Unit_H::DrawDebugPrimitive()
{
    DebugRenderer* debug_renderer = Lemur::Graphics::Graphics::Instance().GetDebugRenderer();
    debug_renderer->DrawBox({ attack_rect.center.x,0, attack_rect.center.y },
        { 0,DirectX::XMConvertToRadians(rect_angle),0 },
        { attack_rect.width.x*0.5f,0.5f, attack_rect.width.y*0.5f }, { 1,0,1,1 });
    debug_renderer->DrawSphere({ attack_rect.right_down.x,0.5f, attack_rect.right_down.y }, 0.1f, { 0,0,1,1 });
    debug_renderer->DrawSphere({ attack_rect.left_up.x,0.5f, attack_rect.left_up.y }, 0.1f, { 0,1,0,1 });

}

void Unit_H::Update(float elapsed_time)
{
    attack_rect.center = { position.x ,position.z };
    // 常に矩形を更新
    attack_rect = CalcRotateRect(attack_rect.center, attack_rect.width, rect_angle);

    Unit::Update(elapsed_time);
}

void Unit_H::UpdateIdleState(float elapsed_time)
{
    EnemyManager& enemyManager = EnemyManager::Instance();
    int enemyCount = enemyManager.GetEnemyCount();

    //TODO モーションが来たらまた変える
    // 敵の総当たり
    for (int j = 0; j < enemyCount; ++j)
    {
        Enemy* enemy = enemyManager.GetEnemy(j);

        // 敵がユニットの攻撃範囲に入っているとき
        if (Collision::IntersectRotateRectVsCircle(
            attack_rect,
            { enemy->GetPosition().x, enemy->GetPosition().z },
            enemy->GetRadius(),
            rect_angle))
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

void Unit_H::UpdateAttackState(float elapsed_time)
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
        if (Collision::IntersectRotateRectVsCircle(
            attack_rect,
            { enemy->GetPosition().x, enemy->GetPosition().z },
            enemy->GetRadius(),
            rect_angle))
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

void Unit_H::DrawDebugGUI(int n)
{
    Character::DrawDebugGUI("Unit_H", n);
}

Unit_I::Unit_I()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Unit\\unit3_RE.fbx");

    attack_times = 5;    // 攻撃回数
    attack_power = 1;    // 攻撃力
    attack_interval = 0.5f; // 攻撃間隔
    attack_collision_range = 1.0f; // 攻撃範囲
    radius = 0.3f; // 半径
    height = 0.5f; // デバッグ用
    dec_pos = 1.0f; // ユニットに接触した種がどのくらい跳ね返されるか
    timer_max = 1.0f; // バフの秒数
    rect_angle = 50.0f; // 矩形角度
    attack_rect.center = { position.x ,position.z };
    attack_rect.width = { 1.0f ,20.0f };
    attack_rect = CalcRotateRect(attack_rect.center, attack_rect.width, rect_angle);

    // とりあえずアニメーション
    model->PlayAnimation(Animation::Idle, true);
}

void Unit_I::DrawDebugPrimitive()
{
    DebugRenderer* debug_renderer = Lemur::Graphics::Graphics::Instance().GetDebugRenderer();
    debug_renderer->DrawBox({ attack_rect.center.x,0, attack_rect.center.y },
        { 0,DirectX::XMConvertToRadians(rect_angle),0 },
        { attack_rect.width.x * 0.5f,0.5f, attack_rect.width.y * 0.5f }, { 1,0,1,1 });
    debug_renderer->DrawSphere({ attack_rect.right_down.x,0.5f, attack_rect.right_down.y }, 0.1f, { 0,0,1,1 });
    debug_renderer->DrawSphere({ attack_rect.left_up.x,0.5f, attack_rect.left_up.y }, 0.1f, { 0,1,0,1 });

}

void Unit_I::Update(float elapsed_time)
{
    attack_rect.center = { position.x ,position.z };
    // 常に矩形を更新
    attack_rect = CalcRotateRect(attack_rect.center, attack_rect.width, rect_angle);

    Unit::Update(elapsed_time);
}

void Unit_I::UpdateIdleState(float elapsed_time)
{
    EnemyManager& enemyManager = EnemyManager::Instance();
    int enemyCount = enemyManager.GetEnemyCount();

    //TODO モーションが来たらまた変える
    // 敵の総当たり
    for (int j = 0; j < enemyCount; ++j)
    {
        Enemy* enemy = enemyManager.GetEnemy(j);

        // 敵がユニットの攻撃範囲に入っているとき
        if (Collision::IntersectRotateRectVsCircle(
            attack_rect,
            { enemy->GetPosition().x, enemy->GetPosition().z },
            enemy->GetRadius(),
            rect_angle))
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

void Unit_I::UpdateAttackState(float elapsed_time)
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
        if (Collision::IntersectRotateRectVsCircle(
            attack_rect,
            { enemy->GetPosition().x, enemy->GetPosition().z },
            enemy->GetRadius(),
            rect_angle))
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

void Unit_I::DrawDebugGUI(int n)
{
    Character::DrawDebugGUI("Unit_I", n);
}


Unit_J::Unit_J()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Unit\\unit1_RE.fbx");

    attack_times = 5;    // 攻撃回数
    attack_power = 1;    // 攻撃力
    attack_interval = 0.5f; // 攻撃間隔
    attack_collision_range = 2.0f; // 攻撃範囲
    attack_radius_in = 1.0f; // 攻撃範囲
    radius = 0.3f; // 半径
    height = 0.5f; // デバッグ用
    dec_pos = 1.0f; // ユニットに接触した種がどのくらい跳ね返されるか

    UpdateTransform();

    // とりあえずアニメーション
    model->PlayAnimation(Animation::Idle, true);
}

void Unit_J::DrawDebugPrimitive()
{
    DebugRenderer* debug_renderer = Lemur::Graphics::Graphics::Instance().GetDebugRenderer();
    debug_renderer->DrawCylinder(position, attack_collision_range, height, { 1,0,1,1 });
    debug_renderer->DrawCylinder(position, attack_radius_in, height, { 1,0,1,1 });
    debug_renderer->DrawCylinder(position, radius, height, { 0,1,0,1 });
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

    //TODO モーションが来たらまた変える
    // 敵の総当たり
    for (int j = 0; j < enemyCount; ++j)
    {
        Enemy* enemy = enemyManager.GetEnemy(j);

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
            is_intersected = true;
            if (is_attack)  enemy->ApplyDamage(ReturnDamage());
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

void Unit_J::DrawDebugGUI(int n)
{
    Character::DrawDebugGUI("Unit_J", n);
}
