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

    // 着地場所より奥、かつ一番距離の近いユニットを探す
    int unitCount = unitManager.GetUnitCount();
    //int near_index = 0;  // 一番近いユニット番号
    //float dis_small = 0.0f; //一番短い距離
    //
    //Unit* unit = nullptr;

    //// ユニット総当たり
    //for (int i = 0; i < unitCount; ++i)
    //{
    //    unit = unitManager.GetUnit(i);

    //    if (unit->position.z >= position.y)//着地場所より奥
    //    {
    //        float d = sqrtf(unit->position.x - position.x * unit->position.x - position.x + unit->position.z - position.y * unit->position.z - position.y);
    //        if (i == 0)// 始めのユニットは代入のみ
    //        {
    //            dis_small = d;
    //            break;
    //        }
    //        if (dis_small > d)// 一番小さい距離を更新する
    //        {
    //            dis_small = d;
    //            near_index = i;
    //        }
    //    }
    //}

    //// 一番近いユニットの後ろに入っていないかどうか確認（入っていればユニットの座標より前の座標を返す）
    //if (unitCount > 0)
    //{
    //    unit = unitManager.GetUnit(near_index);
    //    if (Collision::IntersectSquareVsPoint(unit->square[0], unit->square[1], position))
    //    {
    //        return DirectX::XMFLOAT2(position.x, unit->position.z - unit->dec_pos);
    //    }
    //}
    //return position;

    //// ユニットの総当たり
    //for (int i = 0; i < unitCount; ++i)
    //{
    //    Unit* unit = unitManager.GetUnit(i);
    //    // ユニットの後ろの四角に着地場所があれば、ユニットより少し手前の座標を返す
    //    if (Collision::IntersectSquareVsPoint(unit->square[0], unit->square[1], position))
    //    {
    //        return DirectX::XMFLOAT2(position.x, unit->position.z - unit->dec_pos);
    //    }
    //}

    //// 着地地点に四角がかぶっているユニットをの中で、一番手前にあるものを探す
    //int near_index = 0;  // 手前のユニット番号
    //float near_pos = 0.0f;
    //Unit* unit = nullptr;

    //// ユニット総当たり
    //for (int i = 0; i < unitCount; ++i)
    //{
    //    unit = unitManager.GetUnit(i);

    //    if (Collision::IntersectSquareVsPoint(unit->square[0], unit->square[1], position))
    //    {
    //        if (unit->position.z >= position.y)//着地場所より奥
    //        {
    //            if (i == 0)
    //            {
    //                near_pos = unit->position.z;
    //                break;
    //            }
    //            if (unit->position.z < near_pos)
    //            {
    //                near_pos = unit->position.z;
    //                near_index = i;
    //            }
    //        }
    //    }
    //}

    //// 一番近いユニットの後ろに入っていないかどうか確認（入っていればユニットの座標より前の座標を返す）
    //if (unitCount > 0)
    //{
    //    unit = unitManager.GetUnit(near_index);
    //    if (Collision::IntersectSquareVsPoint(unit->square[0], unit->square[1], position))
    //    {
    //        return DirectX::XMFLOAT2(position.x, unit->position.z - unit->dec_pos);
    //    }
    //}
    
    Unit* unit = nullptr;
    std::vector<int> index;
    int near_index = 0;  // 手前のユニット番号
    float near_pos = 0.0f;
    // ユニット総当たり
    for (int i = 0; i < unitCount; ++i)
    {
        unit = unitManager.GetUnit(i);

        // 着地地点が四角に入っているとき
        if (Collision::IntersectSquareVsPoint(unit->square[0], unit->square[1], position))
        {
            index.push_back(i);// 番号を記録
        }

    }

    if (index.size() == 0)  return position;

    // 記録した番号のユニットを当たる
    for (int j = 0; j < index.size(); ++j)
    {
        unit = unitManager.GetUnit(index.at(j));

        if (j == 0)
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
    if (Collision::IntersectSquareVsPoint(unit->square[0], unit->square[1], position))
    {
        return DirectX::XMFLOAT2(position.x, unit->position.z - unit->dec_pos);
    }

    return position;
}
