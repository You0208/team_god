#include "EnemyManager.h"
#include "Game/Stage/StageManager.h"
#include "Lemur/Graphics/Graphics.h"
#include "Lemur/Collision/Collision.h"

void EnemyManager::Initialize()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();

    model_1 = std::make_unique<FbxModelManager>(graphics.GetDevice(),".\\resources\\Model\\Enemy\\Bear.fbx");
    model_2 = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Enemy\\Bear2.fbx");
    model_3 = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Enemy\\Boar.fbx");
    model_4 = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Enemy\\Boar2.fbx");
    model_1 = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Enemy\\Rabbit.fbx");
    model_2 = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Enemy\\Rabbit2.fbx");
    model_3 = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Enemy\\Deer.fbx");
    model_4 = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Enemy\\Deer.fbx");
}

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
        //TODO ImGui����
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

void EnemyManager::PowerUpEnemy()
{
    for (int i = 0; i < GetEnemyCount(); i++)
    {
        Enemy* enemy = enemies.at(i);
        enemy->PowerUp(add_attack);
    }
}

// �`�揈��
void EnemyManager::Render(float elapsedTime, ID3D11PixelShader* replaced_pixel_shader)
{
    for (Enemy* enemy : enemies)
    {
        enemy->Render(elapsedTime, replaced_pixel_shader);
    }
}

void EnemyManager::Register(Enemy* enemy)
{
    enemy->PlaySetEffect(enemy->GetPosition(), enemy->GetSetEffectSize());
    enemies.emplace_back(enemy);
}

void EnemyManager::Clear()
{
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

bool EnemyManager::NonEnemy()
{
    for (Enemy* enemy : enemies)
    {
        if (enemy->GetEnemyType() != EnemyType::Nuisance)return false;
    }
    return true;
}

void EnemyManager::Remove(Enemy* enemy)
{
    // �j�����X�g�ɒǉ�
    removes.insert(enemy);
}
