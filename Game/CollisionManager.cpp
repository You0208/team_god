#include "CollisionManager.h"
#include <random>

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
            if (unit->GetCategory() == 0 || unit->GetCategory() == 3)
            {
                // 種がユニットの攻撃範囲に入っているとき
                if (Collision::IntersectCircleVsCircle
                ({ seed->GetPosition().x,seed->GetPosition().z },
                    seed->GetRadius(),
                    { unit->GetPosition().x,unit->GetPosition().z },
                    unit->GetAttackCollisionRange()
                ))
                {
                    is_intersected = true;
                    break; // 一度でも重なればループを抜ける
                }
            }
            // 攻撃範囲が三角
            if (unit->GetCategory() == 1 || unit->GetCategory() == 2)
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
            // 攻撃範囲が四角
            if (unit->GetCategory() == 4 || unit->GetCategory() == 5)
            {
                // 種がユニットの攻撃範囲に入っているとき
                if (Collision::IntersectRectVsCircle
                (
                    unit->GetAttackRect(),
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
        if (!is_intersected)
        {
            seed->SetBorn(true);
            break;
        }
        else seed->SetBorn(false);
       
    }
}


// TODO 検討中
DirectX::XMFLOAT2 CollisionManager::CollisionSeedVsSeed(DirectX::XMFLOAT2 position)
{
    // 全ての種と総当たりで衝突判定
    SeedManager& seedtManager = SeedManager::Instance();
    int seedCount = seedtManager.GetSeedCount();
    DirectX::XMFLOAT2 s_p = {};
    if (seedCount < 2)
    {
        return position;
    }
    // 種の総当たり
    for (int i = 0; i < seedCount; ++i)
    {
        Seed* seed = seedtManager.GetSeed(i);
        s_p = { seed->GetPosition().x,seed->GetPosition().z };
        // 着地地点が種の中にある
        if (Collision::IntersectCircleVsPosition(position, s_p, seed->GetRadius()))
        {
            //std::mt19937 mt{ std::random_device{}() };
            //std::uniform_int_distribution<float> width(-1.0f, 1.0f);
            //std::uniform_int_distribution<float> height(0.0f, -1.0f);           
            //DirectX::XMFLOAT2 out_direction = { float(width(mt)),float(height(mt)) };

            float x = (rand() % 20 - 10) * 0.1f;
            float y = (rand() % 10) * -0.1f;

            DirectX::XMFLOAT2 out_direction = { x,y };
            if (0.01f >= out_direction.x && -0.01f >= out_direction.x)  out_direction.x = 1.0f;
            if (0.01f >= out_direction.y && -0.01f >= out_direction.y)  out_direction.y = -1.0f;

            out_direction = Normalize(out_direction);

            return { seed->GetPosition().x + out_direction.x * seed->GetRadius() * 2, seed->GetPosition().z + out_direction.y * seed->GetRadius() * 2 };
        }
    }

    return position;
}

bool CollisionManager::CollisionUnitBackVsSeed_Re(DirectX::XMFLOAT2 position)
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

        // ユニットに直接当たっていたら
        if (Collision::IntersectCircleVsPosition(position, { unit->GetPosition().x,unit->GetPosition().z }, unit->GetRadius()))
        {
            return true;
        }
        // 着地地点が四角に入っているとき
        if (Collision::IntersectSquareVsPoint(unit->GetRect().left_up, unit->GetRect().right_down, position))
        {
            index.push_back(i);// 番号を記録
        }
    }

    // 該当ユニットがなければ終了
    if (index.size() == 0)  return false;

    // 記録した番号のユニットを当たる
    for (int j = 0; j < index.size(); ++j)
    {
        unit = unitManager.GetUnit(index.at(j));

        if (j == 0)// 始めは比較なし
        {
            near_pos = unit->GetPosition().z;
            break;
        }
        else if (unit->GetPosition().z < near_pos)
        {
            near_pos = unit->GetPosition().z;
            near_index = j;
        }
    }

    unit = unitManager.GetUnit(index.at(near_index));
    if (Collision::IntersectSquareVsPoint(unit->GetRect().left_up, unit->GetRect().right_down, position))
    {
        return true;
    }

}

bool CollisionManager::CollisionUnitBackVsSeed_Re(DirectX::XMFLOAT2 position, DirectX::XMFLOAT2& dis_pos)
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
        if (Collision::IntersectSquareVsPoint(unit->GetRect().left_up, unit->GetRect().right_down, position))
        {
            index.push_back(i);// 番号を記録
        }
    }

    // 該当ユニットがなければ終了
    if (index.size() == 0)  return false;

    // 記録した番号のユニットを当たる（種が範囲に接触したユニットの総当たり）
    for (int j = 0; j < index.size(); ++j)
    {
        unit = unitManager.GetUnit(index.at(j));

        if (j == 0)// 始めは比較なし
        {
            near_pos = unit->GetPosition().z;
        }
        else if (unit->GetPosition().z < near_pos)
        {
            near_pos = unit->GetPosition().z;
            near_index = j;
        }
    }

    unit = unitManager.GetUnit(index.at(near_index));
    if (Collision::IntersectSquareVsPoint(unit->GetRect().left_up, unit->GetRect().right_down, position))
    {
        dis_pos = { unit->GetPosition().x,unit->GetPosition().z };
        return true;
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
        if (Collision::IntersectSquareVsPoint(unit->GetRect().left_up, unit->GetRect().right_down, position))
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
            near_pos = unit->GetPosition().z;
            break;
        }
        else if (unit->GetPosition().z < near_pos)
        {
            near_pos = unit->GetPosition().z;
            near_index = j;
        }
    }

    unit = unitManager.GetUnit(index.at(near_index));
    if (Collision::IntersectSquareVsPoint(unit->GetRect().left_up, unit->GetRect().right_down, position))
    {
        return DirectX::XMFLOAT2(position.x, unit->GetPosition().z - unit->GetDecPos());
    }

    return position;
}
