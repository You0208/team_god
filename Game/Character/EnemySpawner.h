#pragma once
#include "Enemy.h"

class EnemySpawner
{
private:
    EnemySpawner() {};
    ~EnemySpawner() {}
public:
    // 唯一のインスタンス取得
    static EnemySpawner& Instance()
    {
        static EnemySpawner instance;
        return instance;
    };

    void Initialize();

    void Update(float elapsed_time);

    EnemyScript SetEnemy(float second, int enemy_type, int shaft, float pos);

    void InitializeLevel1();

    // エネミーの登録
    void EnemySpawn(EnemyScript script);

private:
    std::vector<EnemyScript>    level_1         = {};    // レベル１

    int index;
    float                       timer          = 0.0f;  // タイマー
    float                       stage_to_shaft = 1.0f;  // ステージと軸の離れ具合
    DirectX::XMFLOAT2           shaft_pos      = {};    // 各軸の位置
    DirectX::XMFLOAT2           stage_pos      = {0,0};    // ステージ位置の保存
    DirectX::XMFLOAT2           stage_width    = {5,5};    // ステージ幅の保存
};
