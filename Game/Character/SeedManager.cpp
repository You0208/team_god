#include "SeedManager.h"
#include "UnitManager.h"
#include "Lemur/Graphics/Graphics.h"
#include "Lemur/Collision/Collision.h"
#include "Game/CollisionManager.h"

// 更新処理
void SeedManager::Update(float elapsedTime)
{
    for (Seed* seed : seeds)
    {
        // 時間切れの時
        if (seed->GetTimer() >= seed_limit_time)
        {
            // 死亡に切り替え
            seed->death = true;
        }
        else
        {
            // 種の更新
            seed->Update(elapsedTime);
        }

        // 死亡時の処理
        if (seed->death)
        {
            for (Seed* s : seeds)
            {
                // もし削除予定の種番号よりも大きい番号の種があれば、番号を減算する
                if (seed->GetNumber() < s->GetNumber())s->DecNumber();
            }
            // 現在の種番号を減算
            seed_number--;

            // 種をリストから消す
            Remove(seed);
        }
    }


    // 破棄処理
    for (Seed* seed : removes)
    {
        std::vector<Seed*>::iterator it = std::find(seeds.begin(), seeds.end(), seed);
        if (it != seeds.end())
        {
            seeds.erase(it);
        }

        // 削除
        delete seed;
    }
    // 破棄リストをクリア
    removes.clear();
}

// 描画処理
void SeedManager::Render(float elapsedTime, ID3D11PixelShader** replaced_pixel_shader)
{
    DrawDebugPrimitive();
    for (Seed* seed : seeds)
    {
        seed->Render(elapsedTime, replaced_pixel_shader);
    }
}

void SeedManager::Register(Seed* seed)
{
    // 初期値を設定
    seed->SetNumber(seed_number);

    seeds.emplace_back(seed);
    // 番号を加算
    seed_number++;
}

void SeedManager::Clear()
{
    for (Seed* seed : seeds)
    {
        delete seed;
    }
    seeds.clear();
}

void SeedManager::DrawDebugPrimitive()
{
    for (Seed* seed : seeds)
    {
        seed->DrawDebugPrimitive();
    }
}

void SeedManager::Remove(Seed* seed)
{
    // 破棄リストに追加
    removes.insert(seed);
}
