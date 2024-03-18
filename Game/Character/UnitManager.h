#pragma once
#pragma once

#include<vector>
#include<set>
#include "Unit.h"

// ユニットマネージャー
class UnitManager
{
private:
    UnitManager() {}
    ~UnitManager() {}
public:
    // 唯一のインスタンス取得
    static UnitManager& Instance()
    {
        static UnitManager instance;
        return instance;
    }

    // 更新処理
    void Update(float elapsedTime);

    // 描画処理
    void Render(float elapsedTime, ID3D11PixelShader** replaced_pixel_shader);

    // ユニット登録
    void Register(Unit* unit);

    // ユニット全削除
    void Clear();

    //デバッグプリミティブ描画
    void DrawDebugPrimitive();

    // ユニット数取得
    int GetEnemyCount() const { return static_cast<int>(units.size()); }

    // ユニット取得
    Unit* GetEnemy(int index) { return units.at(index); }

    // ユニット同士の衝突判定
    void CollisionEnemyVsEnemis();

    // ユニット削除
    void Remove(Unit* units);

private:
    std::vector<Unit*> units;
    std::set<Unit*>    removes;
};