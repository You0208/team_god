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

    void InitializeLevel1();

    // �G�l�~�[�̓o�^
    void EnemySpawn(EnemyScript script);

private:
    std::vector<EnemyScript>    level_1         = {};    // ���x���P

    int index;
    float                       timer          = 0.0f;  // �^�C�}�[
    float                       stage_to_shaft = 1.0f;  // �X�e�[�W�Ǝ��̗���
    DirectX::XMFLOAT2           shaft_pos      = {};    // �e���̈ʒu
    DirectX::XMFLOAT2           stage_pos      = {0,0};    // �X�e�[�W�ʒu�̕ۑ�
    DirectX::XMFLOAT2           stage_width    = {5,5};    // �X�e�[�W���̕ۑ�
};
