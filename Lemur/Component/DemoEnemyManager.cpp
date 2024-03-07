#include "DemoEnemyManager.h"


void DemoEnemyManager::Update(float elapsedTime)
{
    for (DemoEnemy* enemy : enemies)
    {
        enemy->Update(elapsedTime);
    }

    // 破棄処理
    for (DemoEnemy* enemy : removes)
    {
        std::vector<DemoEnemy*>::iterator it = std::find(enemies.begin(), enemies.end(), enemy);
        if (it != enemies.end())
        {
            enemies.erase(it);
        }

        // 削除
        delete enemy;
    }
    // 破棄リストをクリア
    removes.clear();
}

void DemoEnemyManager::Render(float elapsedTime)
{
    for (DemoEnemy* enemy : enemies)
    {
        enemy->Render(elapsedTime);
    }
}

void DemoEnemyManager::Register(DemoEnemy* enemy)
{
    enemies.emplace_back(enemy);
}

void DemoEnemyManager::Clear()
{
    for (DemoEnemy* enemy : enemies)
    {
        enemy->Delete();
        delete enemy;
    }
    enemies.clear();
}

void DemoEnemyManager::DrawDebugPrimitive()
{
}

void DemoEnemyManager::CollisionEnemyVsEnemis()
{
}

void DemoEnemyManager::Remove(DemoEnemy* enemy)
{
    // 破棄リストに追加
    removes.insert(enemy);
}
