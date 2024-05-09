#include "EnemyManager.h"
#include "Game/Stage/StageManager.h"
#include "Lemur/Graphics/Graphics.h"
#include "Lemur/Collision/Collision.h"

// �X�V����
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

    // �j������
    for (Enemy* enemy : removes)
    {
        std::vector<Enemy*>::iterator it = std::find(enemies.begin(), enemies.end(), enemy);
        if (it != enemies.end())
        {
            enemies.erase(it);
        }

        // �폜
        delete enemy;
    }
    // �j�����X�g���N���A
    removes.clear();
}

// �`�揈��
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
    // �j�����X�g�ɒǉ�
    removes.insert(enemy);
}
