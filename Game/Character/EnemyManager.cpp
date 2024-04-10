#include "EnemyManager.h"
#include "Lemur/Graphics/Graphics.h"
#include "Lemur/Collision/Collision.h"


// �X�V����
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


    // �G���m�̓����蔻��
    CollisionEnemyVsEnemis();
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

// �G�l�~�[���m�̏Փ˔���
void EnemyManager::CollisionEnemyVsEnemis()
{
    EnemyManager& enemyManager = EnemyManager::Instance();

    // �S�Ă̓G�Ƒ�������ŏՓ˔���
    int enemyCount = enemies.size();
    for (int i = 0; i < enemyCount; ++i)
    {
        Enemy* enemyA = enemies.at(i);
        for (int j = i + 1; j < enemyCount; ++j)
        {
            Enemy* enemyB = enemies.at(j);
            // �Փ˔���
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
    // �j�����X�g�ɒǉ�
    removes.insert(enemy);
}
