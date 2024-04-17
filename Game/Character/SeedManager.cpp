#include "SeedManager.h"
#include "UnitManager.h"
#include "Game/Stage/Fence.h"
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
            seed->SetDead(true);
        }
        else
        {
            // 種の更新
            seed->Update(elapsedTime);
        }

        // 死亡時の処理
        if (seed->IsDead())
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

    // 種VSユニット
    SeedVsUnit();
    // 種VS柵
    SeedVsFence();
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

void SeedManager::SeedVsUnit()
{
    SeedManager& seedManager = SeedManager::Instance();
    UnitManager& unitManager = UnitManager::Instance();

    // 全ての種と総当たりで衝突判定
    int seedCount = seedManager.GetSeedCount();
    int unitCount = unitManager.GetUnitCount();

    for (int i = 0; i < seedCount; ++i)
    {
        Seed* seed = seedManager.GetSeed(i);
        for (int j = 0; j < unitCount; ++j)
        {
            Unit* unit = unitManager.GetUnit(j);

            // ユニットと種が当たっていたら消す
            if (Collision::IntersectCircleVsCircle(
                { seed->GetPosition().x, seed->GetPosition().z },
                seed->GetRadius(),
                { unit->GetPosition().x, unit->GetPosition().z },
                unit->GetRadius()
            ))
            {
                seed->SetDead(true);
            }
        }
    }

}

void SeedManager::SeedVsFence()
{
    SeedManager& seedManager = SeedManager::Instance();
    // 全ての種と総当たりで衝突判定
    int seedCount = seedManager.GetSeedCount();

    for (int i = 0; i < seedCount; ++i)
    {
        Seed* seed = seedManager.GetSeed(i);
        // 柵と種が当たっていたら消す
        if (Collision::IntersectCircleVsCircle(
            { seed->GetPosition().x, seed->GetPosition().z },
            seed->GetRadius(),
            { Fence::Instance().GetPosition().x, Fence::Instance().GetPosition().z },
            Fence::Instance().GetRadius()
        ))
        {
            seed->SetDead(true);
        }
    }

}
