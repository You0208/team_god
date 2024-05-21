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
        //TODO ImGui消す
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

void EnemyManager::PowerUpEnemy()
{
    for (int i = 0; i < GetEnemyCount(); i++)
    {
        Enemy* enemy = enemies.at(i);
        enemy->PowerUp(add_attack);
    }
}

// 描画処理
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
    // 破棄リストに追加
    removes.insert(enemy);
}
