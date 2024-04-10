#include "EnemyManager.h"
#include "Lemur/Graphics/Graphics.h"
#include "Lemur/Collision/Collision.h"


// 更新処理
void EnemyManager::Update(float elapsedTime)
{
    for (Enemy* enemy : enemies)
    {
        enemy->Update(elapsedTime);
    }

    for (int i = 0; i < GetEnemyCount(); i++)
    {
        Enemy* enemy = enemies.at(i);
        enemy->DrawDebugGUI(i);
    }

    // 破棄処理
    for (Enemy* enemy : removes)
    {
        std::vector<Enemy*>::iterator it = std::find(enemies.begin(), enemies.end(), enemy);
        if (it != enemies.end())
        {
            enemies.erase(it);
        }

        // 削除
        delete enemy;
    }
    // 破棄リストをクリア
    removes.clear();


    // 敵同士の当たり判定
    CollisionEnemyVsEnemis();
}

// 描画処理
void EnemyManager::Render(float elapsedTime, ID3D11PixelShader** replaced_pixel_shader)
{
    for (Enemy* enemy : enemies)
    {
        enemy->Render(elapsedTime, replaced_pixel_shader);
    }
}

void EnemyManager::Register(Enemy* enemy)
{
    enemies.emplace_back(enemy);
}

void EnemyManager::Clear()
{
    for (Enemy* enemy : enemies)
    {
        delete enemy;
    }
    enemies.clear();
}

void EnemyManager::DrawDebugPrimitive()
{
    for (Enemy* enemy : enemies)
    {
        enemy->DrawDebugPrimitive();
    }
}

// エネミー同士の衝突判定
void EnemyManager::CollisionEnemyVsEnemis()
{
    EnemyManager& enemyManager = EnemyManager::Instance();

    // 全ての敵と総当たりで衝突判定
    int enemyCount = enemies.size();
    for (int i = 0; i < enemyCount; ++i)
    {
        Enemy* enemyA = enemies.at(i);
        for (int j = i + 1; j < enemyCount; ++j)
        {
            Enemy* enemyB = enemies.at(j);
            // 衝突判定
            DirectX::XMFLOAT3 outPosition;
            if (Collision::IntersectCylinderVsCylinder
            (enemyA->GetPosition(),
                enemyA->GetRadius(),
                enemyA->GetHeight(),
                enemyB->GetPosition(),
                enemyB->GetRadius(),
                enemyB->GetHeight(),
                outPosition)
                )
            {
                enemyB->SetPosition(outPosition);
            }
        }
    }
}

void EnemyManager::Remove(Enemy* enemy)
{
    // 破棄リストに追加
    removes.insert(enemy);
}
