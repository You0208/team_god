#pragma once

#include<vector>
#include<set>
#include "Seed.h"

// 種マネージャー
class SeedManager
{
private:
    SeedManager() {}
    ~SeedManager() {}
public:
    // 唯一のインスタンス取得
    static SeedManager& Instance()
    {
        static SeedManager instance;
        return instance;
    }

    // 更新処理
    void Update(float elapsedTime);

    // 描画処理
    void Render(float elapsedTime, ID3D11PixelShader** replaced_pixel_shader);

    // エネミー登録
    void Register(Seed* enemy);

    // エネミー全削除
    void Clear();

    //デバッグプリミティブ描画
    void DrawDebugPrimitive();

    // エネミー数取得
    int GetEnemyCount() const { return static_cast<int>(seeds.size()); }

    // エネミー取得
    Seed* GetEnemy(int index) { return seeds.at(index); }

    // エネミー同士の衝突判定
    void CollisionSeedVsSeeds();

    // エネミー削除
    void Remove(Seed* enemy);

private:
    std::vector<Seed*> seeds;
    std::set<Seed*>    removes;

    int seed_number = 0;
};