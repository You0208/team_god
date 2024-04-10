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

            // 攻撃範囲が円
            if (unit->category == 0 || unit->category == 3)
            {
                // 種がユニットの攻撃範囲に入っているとき
                if (Collision::IntersectCircleVsCircle
                ({ seed->GetPosition().x,seed->GetPosition().z },
                    seed->GetRadius(),
                    { unit->GetPosition().x,unit->GetPosition().z },
                    unit->GetAttackRadius()
                ))
                {
                    is_intersected = true;
                    break; // 一度でも重なればループを抜ける
                }
            }
            // 攻撃範囲が三角
            if (unit->category == 1 || unit->category == 2)
            {
                // 種がユニットの攻撃範囲に入っているとき
                if (Collision::IntersectTriangleVsCircle
                (
                    unit->GetTriangle1(),
                    { seed->GetPosition().x,seed->GetPosition().z },
                    seed->GetRadius()
                    ))
                {
                    is_intersected = true;
                    break; // 一度でも重なればループを抜ける
                }
                // 種がユニットの攻撃範囲に入っているとき
                if (Collision::IntersectTriangleVsCircle
                (
                    unit->GetTriangle2(),
                    { seed->GetPosition().x,seed->GetPosition().z },
                    seed->GetRadius()
                ))
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

DirectX::XMFLOAT2 CollisionManager::CollisionUnitBackVsSeed(DirectX::XMFLOAT2 position)
{
    UnitManager& unitManager = UnitManager::Instance();

    // 着地場所より奥、かつ一番距離の近いユニットを探す
    int unitCount = unitManager.GetUnitCount();
  
    Unit* unit = nullptr;   
    std::vector<int> index; // 着地点に入ってるユニット番号
    int near_index = 0;     // 手前のユニット番号
    float near_pos = 0.0f;  // 一番手前の座標

    // ユニット総当たり
    for (int i = 0; i < unitCount; ++i)
    {
        unit = unitManager.GetUnit(i);

        // 着地地点が四角に入っているとき
        if (Collision::IntersectSquareVsPoint(unit->square.left_up, unit->square.right_down, position))
        {
            index.push_back(i);// 番号を記録
        }
    }

    // 該当ユニットがなければ終了
    if (index.size() == 0)  return position;

    // 記録した番号のユニットを当たる
    for (int j = 0; j < index.size(); ++j)
    {
        unit = unitManager.GetUnit(index.at(j));

        if (j == 0)// 始めは比較なし
        {
            near_pos = unit->position.z;
            break;
        }
        else if (unit->position.z < near_pos)
        {
            near_pos = unit->position.z;
            near_index = j;
        }
    }

    unit = unitManager.GetUnit(index.at(near_index));
    if (Collision::IntersectSquareVsPoint(unit->square.left_up, unit->square.right_down, position))
    {
        return DirectX::XMFLOAT2(position.x, unit->position.z - unit->dec_pos);
    }

    return position;
}
