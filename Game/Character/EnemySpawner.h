#pragma once
#include "Enemy.h"

class EnemySpawner
{
private:
    EnemySpawner() {};
    ~EnemySpawner() {}
public:
    // �B��̃C���X�^���X�擾
    static EnemySpawner& Instance()
    {
        static EnemySpawner instance;
        return instance;
    };

    void Initialize();

    void Update(float elapsed_time);

    EnemyScript SetEnemy(float second, int enemy_type, int shaft, float pos);
    EnemyScript SetEnemy(float second, int enemy_type, DirectX::XMFLOAT2 pos);

    // ���x���P�̏�����
    void InitializeLevel1();//1-1
    void InitializeLevel2();//1-2
    void InitializeLevel3();//1-3
    void InitializeLevel4();//2-1
    void InitializeLevel5();//2-2
    void InitializeLevel6();//2-3
    void InitializeLevel7();//3-1
    void InitializeLevel8();//3-2
    void InitializeLevel9();//3-3

    // �G�l�~�[�̓o�^
    void EnemySpawn(EnemyScript script);

    // Imgui
    void DebugImGui();
private:
    std::vector<EnemyScript>    level_1 = {};    // ���x���P
    std::vector<EnemyScript>    level_2 = {};    // ���x���P
    std::vector<EnemyScript>    level_3 = {};    // ���x���P
    std::vector<EnemyScript>    level_4 = {};    // ���x���P
    std::vector<EnemyScript>    level_5 = {};    // ���x���P
    std::vector<EnemyScript>    level_6 = {};    // ���x���P
    std::vector<EnemyScript>    level_7 = {};    // ���x���P
    std::vector<EnemyScript>    level_8 = {};    // ���x���P
    std::vector<EnemyScript>    level_9 = {};    // ���x���P

    int index;
    float                       timer          = 0.0f;  // �^�C�}�[
    float                       timer_speed    = 3.0f;  // �^�C�}�[�̑�������{
    float                       stage_to_shaft = 1.0f;  // �X�e�[�W�Ǝ��̗���
    DirectX::XMFLOAT2           shaft_pos      = {};    // �e���̈ʒu
    DirectX::XMFLOAT2           stage_pos      = {0,0};    // �X�e�[�W�ʒu�̕ۑ�
    DirectX::XMFLOAT2           stage_width    = {5,5};    // �X�e�[�W���̕ۑ�



//-----���̉��f�o�b�O�p��--------------------------------------------
public:
    struct EnemyStatus
    {
        int attack_power;         //�U����
        float attack_interval;    //�U���Ԋu
        int health;               // HP
        float speed_power;        // ���x
        float radius;             // ���a
        float attack_effect_size; // �U���G�t�F�N�g�̃T�C�Y
        float death_effect_size;  // ���S�G�t�F�N�g�̃T�C�Y
        float hit_effect_size;    // �ݒu�G�t�F�N�g�̃T�C�Y
    };

    // �Z�b�g
    static void SetBasicEnemyStatus(Enemy* enemy, EnemyStatus status);

private:
    //Enemy_A
    EnemyStatus enemy_A;

    // Enemy_B
    EnemyStatus enemy_B;
    float mover_timer_max_B = 1.0f; // ��C�ɐi�ނ܂ł̎���
    float dis_B = 3.0f;     // ��C�ɐi�ދ���

    // Enemy_C
    EnemyStatus enemy_C;

    // Enemy_D
    EnemyStatus enemy_D;
    float speed_power_Y_D = -1.0f;
    float dis_D = 1.0f;

    //Enemy_A
    EnemyStatus enemy_A_2;

    // Enemy_B
    EnemyStatus enemy_B_2;
    float mover_timer_max_B_2 = 1.0f; // ��C�ɐi�ނ܂ł̎���
    float dis_B_2 = 3.0f;     // ��C�ɐi�ދ���

    // Enemy_C
    EnemyStatus enemy_C_2;

    // Enemy_D
    EnemyStatus enemy_D_2;
    float speed_power_Y_D_2 = -1.0f;
    float dis_D_2 = 1.0f;

    // SummonEnemy
    EnemyStatus summon_enemy;

    // BossEnemy
    EnemyStatus boss_enemy;

    // Imgui
    void EnemyImGui(EnemyStatus& status);

    // �f�o�b�O�p�X�N���v�g
    EnemyScript script_enemy_A;
    EnemyScript script_enemy_B;
    EnemyScript script_enemy_C;
    EnemyScript script_enemy_D;
    EnemyScript script_enemy_A_2;
    EnemyScript script_enemy_B_2;
    EnemyScript script_enemy_C_2;
    EnemyScript script_enemy_D_2;

    EnemyScript script_summon_enemy;
    EnemyScript script_boss_enemy;

    int count;
};
