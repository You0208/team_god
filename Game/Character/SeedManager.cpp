#include "SeedManager.h"
#include "Lemur/Graphics/Graphics.h"
#include "Lemur/Collision/Collision.h"

// 更新処理
void SeedManager::Update(float elapsedTime)
{
    // 時間切れの種を消す（seed->deathがtrueのとき）
    seeds.erase(std::remove_if(seeds.begin(), seeds.end(), [](Seed* seed) {return seed->death; }), seeds.end());

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

// エネミー同士の衝突判定
void SeedManager::CollisionSeedVsSeeds()
{
    SeedManager& seedManager = SeedManager::Instance();

    // 全ての敵と総当たりで衝突判定
    int seedCount = seeds.size();
    for (int i = 0; i < seedCount; ++i)
    {
        Seed* seedA = seeds.at(i);
        for (int j = i + 1; j < seedCount; ++j)
        {
            Seed* seedB = seeds.at(j);
            // 衝突判定
            DirectX::XMFLOAT3 outPosition;
            if (Collision::IntersectCylinderVsCylinder
            (seedA->GetPosition(),
                seedA->GetRadius(),
                seedA->GetHeight(),
                seedB->GetPosition(),
                seedB->GetRadius(),
                seedB->GetHeight(),
                outPosition)
                )
            {
                seedB->SetPosition(outPosition);
            }
        }
    }
}

void SeedManager::Remove(Seed* seed)
{
    // 破棄リストに追加
    removes.insert(seed);
}
