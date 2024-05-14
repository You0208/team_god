#pragma once

#include<vector>
#include<set>
#include "Enemy.h"

// エネミーマネージャー
class EnemyManager
{
private:
    EnemyManager() {}
    ~EnemyManager() {}
public:
    // 唯一のインスタンス取得
    static EnemyManager& Instance()
    {
        static EnemyManager instance;
        return instance;
    }
    // 更新処理
    void Update(float elapsedTime);

    void PowerUpEnemy();

    // 描画処理
    void Render(float elapsedTime, ID3D11PixelShader** replaced_pixel_shader);

    // エネミー登録
    void Register(Enemy* enemy);

    // エネミー全削除
    void Clear();

    //デバッグプリミティブ描画
    void DrawDebugPrimitive();

    // エネミー数取得
    int GetEnemyCount() const { return static_cast<int>(enemies.size()); }

    // エネミー取得
    Enemy* GetEnemy(int index) { return enemies.at(index); }

    // エネミー削除
    void Remove(Enemy* enemy);

    void SetTimeUp(bool time_up_) { time_up = time_up_; }
    bool GetTimeUp() { return time_up; }
private:
    std::vector<Enemy*> enemies = {};
    std::set<Enemy*>    removes = {};
    DirectX::XMFLOAT2   shaft_pos = {};    // 各軸の位置

    int add_attack = 2;
    bool time_up = true;
};