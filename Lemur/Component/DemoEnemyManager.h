#pragma once

#include<vector>
#include<set>
#include "DemoEnemy.h"

// エネミーマネージャー
class DemoEnemyManager
{
private:
    DemoEnemyManager() {}
    ~DemoEnemyManager() {}
public:
    // 唯一のインスタンス取得
    static DemoEnemyManager& Instance()
    {
        static DemoEnemyManager instance;
        return instance;
    }

    // 更新処理
    void Update(float elapsedTime);

    // 描画処理
    void Render(float elapsedTime);

    // エネミー登録
    void Register(DemoEnemy* enemy);

    // エネミー全削除
    void Clear();

    //デバッグプリミティブ描画
    void DrawDebugPrimitive();

    // エネミー数取得
    int GetEnemyCount() const { return static_cast<int>(enemies.size()); }

    // エネミー取得
    DemoEnemy* GetEnemy(int index) { return enemies.at(index); }

    // エネミー同士の衝突判定
    void CollisionEnemyVsEnemis();

    // エネミー削除
    void Remove(DemoEnemy* enemy);

private:
    std::vector<DemoEnemy*> enemies;
    std::set<DemoEnemy*>    removes;
};
