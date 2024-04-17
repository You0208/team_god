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

    // 種登録
    void Register(Seed* enemy);

    // 種全削除
    void Clear();

    //デバッグプリミティブ描画
    void DrawDebugPrimitive();

    // 種数取得
    int GetSeedCount() const { return static_cast<int>(seeds.size()); }

    // 種取得
    Seed* GetSeed(int index) { return seeds.at(index); }

    // 種削除
    void Remove(Seed* enemy);

    // 種とユニットの当たり判定
    void SeedVsUnit();

    // 種と柵の当たり判定
    void SeedVsFence();

    int seed_number = 0;// 次の種番号
private:
    std::vector<Seed*> seeds;
    std::set<Seed*>    removes;

    float seed_limit_time = 15.0f;// 種の制限時間
};