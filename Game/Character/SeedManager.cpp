#include "SeedManager.h"
#include "UnitManager.h"
#include "Lemur/Graphics/Graphics.h"
#include "Lemur/Collision/Collision.h"

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

    CollisionSeedVsUnit();

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

// 種とユニットの当たり判定
void SeedManager::CollisionSeedVsUnit()
{
    UnitManager& unitManager = UnitManager::Instance();

    // 全ての敵と総当たりで衝突判定
    int unitCount = unitManager.GetUnitCount();
    bool is_intersected =false;

    // 種の総当たり
    for (Seed* seed : seeds)
    {
        is_intersected = false;
        // ユニットの総当たり
        for (int i = 0; i < unitCount; ++i)
        {
            Unit* unit = unitManager.GetUnit(i);

            if (unit->category==0||unit->category==3)
            {
                // 種がユニットの範囲に入っているとき
                if (Collision::IntersectCylinderVsCylinder
                (seed->GetPosition(),
                    seed->GetRadius(),
                    seed->GetHeight(),
                    unit->GetPosition(),
                    unit->GetRadius(),
                    unit->GetHeight())
                    )
                {
                    is_intersected = true;
                    break; // 一度でも重なればループを抜ける
                }
            }
        }

        // 重なりがない場合、種を生まれた状態に設定
        if (!is_intersected)seed->SetBorn(true);
    }
}

void SeedManager::Remove(Seed* seed)
{
    // 破棄リストに追加
    removes.insert(seed);
}
