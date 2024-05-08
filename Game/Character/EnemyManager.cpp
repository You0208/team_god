#include "EnemyManager.h"
#include "Game/Stage/StageManager.h"
#include "Lemur/Graphics/Graphics.h"
#include "Lemur/Collision/Collision.h"

// 更新処理
void EnemyManager::Update(float elapsedTime)
{
    for (int i = 0; i < GetEnemyCount(); i++)
    {
        Enemy* enemy = enemies.at(i);
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

void EnemyManager::Remove(Enemy* enemy)
{
    // 破棄リストに追加
    removes.insert(enemy);
}
