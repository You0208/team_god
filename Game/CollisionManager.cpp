#include "CollisionManager.h"

void CollisionManager::CollisionSeedVsUnit()
{
    UnitManager& unitManager = UnitManager::Instance();
    SeedManager& seedtManager = SeedManager::Instance();

    // 全ての敵と総当たりで衝突判定
    int unitCount = unitManager.GetUnitCount();
    int seedCount = seedtManager.GetSeedCount();

    bool is_intersected = false;

    // 種の総当たり
    for (int j = 0; j < seedCount; ++j)
    {
        Seed* seed = seedtManager.GetSeed(j);

        is_intersected = false;
        // ユニットの総当たり
        for (int i = 0; i < unitCount; ++i)
        {
            Unit* unit = unitManager.GetUnit(i);

            if (unit->category == 0 || unit->category == 3)
            {
                // 種がユニットの攻撃範囲に入っているとき
                if (Collision::IntersectCylinderVsCylinder
                (seed->GetPosition(),
                    seed->GetRadius(),
                    seed->GetHeight(),
                    unit->GetPosition(),
                    unit->GetAttackRadius(),
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


//TODO まず一番自分に近いユニットを算出する必要がある
DirectX::XMFLOAT2 CollisionManager::CollisionUnitBackVsSeed(DirectX::XMFLOAT2 position)
{
    UnitManager& unitManager = UnitManager::Instance();

    // 全ての敵と総当たりで衝突判定
    int unitCount = unitManager.GetUnitCount();

    // ユニットの総当たり
    for (int i = 0; i < unitCount; ++i)
    {
        Unit* unit = unitManager.GetUnit(i);
        // ユニットの後ろの四角に着地場所があれば、ユニットより少し手前の座標を返す
        if (Collision::IntersectSquareVsPoint(unit->square[0], unit->square[1], position))
        {
            return DirectX::XMFLOAT2(position.x, position.y - unit->dec_pos);
        }
    }

    return position;
}
