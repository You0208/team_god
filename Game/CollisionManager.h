#pragma once

#include<vector>
#include<set>
#include "Character/SeedManager.h"
#include "Character/UnitManager.h"
#include "Character/EnemyManager.h"
#include "Lemur/Collision/Collision.h"

// 当たり判定マネージャー
class CollisionManager
{
private:
    CollisionManager() {}
    ~CollisionManager() {}
public:
    // 唯一のインスタンス取得
    static CollisionManager& Instance()
    {
        static CollisionManager instance;
        return instance;
    }

    // 種とユニットの当たり判定
    void CollisionSeedVsUnit();

    // 種の着地場所に種が無いか確認（あれば帰ってきた値を着地場所にする）
    DirectX::XMFLOAT2 CollisionSeedVsSeed(DirectX::XMFLOAT2 position);

    static bool CollisionUnitBackVsSeed_Re(DirectX::XMFLOAT2 position);

    // 種の着地場所の前にユニットが無いか確認（あれば帰ってきた値を着地場所にする）
    DirectX::XMFLOAT2 CollisionUnitBackVsSeed(DirectX::XMFLOAT2 position);
};