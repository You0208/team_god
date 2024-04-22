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

    // レベル１の初期化
    void InitializeLevel1();
    void InitializeLevel2();

    // エネミーの登録
    void EnemySpawn(EnemyScript script);

    // Imgui
    void DebugImGui();
private:
    std::vector<EnemyScript>    level_1         = {};    // レベル１
    std::vector<EnemyScript>    level_2         = {};    // レベル２

    int index;
    float                       timer          = 0.0f;  // タイマー
    float                       stage_to_shaft = 1.0f;  // ステージと軸の離れ具合
    DirectX::XMFLOAT2           shaft_pos      = {};    // 各軸の位置
    DirectX::XMFLOAT2           stage_pos      = {0,0};    // ステージ位置の保存
    DirectX::XMFLOAT2           stage_width    = {5,5};    // ステージ幅の保存


//-----この下デバッグ用↓--------------------------------------------
    struct EnemyStatus
    {
        int attack_power;//攻撃力
        float attack_interval;//攻撃間隔
        int health; // HP
        float speed_power;// 速度
        float radius;  // 半径
    };

    //Enemy_A
    EnemyStatus enemy_A;

    // Enemy_B
    EnemyStatus enemy_B;
    float mover_timer_max_B = 1.0f; // 一気に進むまでの時間
    float dis_B = 3.0f;     // 一気に進む距離

    // Enemy_C
    EnemyStatus enemy_C;

    // Enemy_D
    EnemyStatus enemy_D;
    float speed_power_Y_D = -1.0f;
    float dis_D = 1.0f;

    // セット
    void SetBasicEnemyStatus(Enemy* enemy, EnemyStatus status);
    // Imgui
    void EnemyImGui(EnemyStatus& status);

    // デバッグ用スクリプト
    EnemyScript script_enemy_A;
    EnemyScript script_enemy_B;
    EnemyScript script_enemy_C;
    EnemyScript script_enemy_D;

    int count;
};
