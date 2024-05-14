#pragma once

#include<vector>
#include<set>
#include "Enemy.h"

// �G�l�~�[�}�l�[�W���[
class EnemyManager
{
private:
    EnemyManager() {}
    ~EnemyManager() {}
public:
    // �B��̃C���X�^���X�擾
    static EnemyManager& Instance()
    {
        static EnemyManager instance;
        return instance;
    }
    // �X�V����
    void Update(float elapsedTime);

    void PowerUpEnemy();

    // �`�揈��
    void Render(float elapsedTime, ID3D11PixelShader** replaced_pixel_shader);

    // �G�l�~�[�o�^
    void Register(Enemy* enemy);

    // �G�l�~�[�S�폜
    void Clear();

    //�f�o�b�O�v���~�e�B�u�`��
    void DrawDebugPrimitive();

    // �G�l�~�[���擾
    int GetEnemyCount() const { return static_cast<int>(enemies.size()); }

    // �G�l�~�[�擾
    Enemy* GetEnemy(int index) { return enemies.at(index); }

    // �G�l�~�[�폜
    void Remove(Enemy* enemy);

    void SetTimeUp(bool time_up_) { time_up = time_up_; }
    bool GetTimeUp() { return time_up; }
private:
    std::vector<Enemy*> enemies = {};
    std::set<Enemy*>    removes = {};
    DirectX::XMFLOAT2   shaft_pos = {};    // �e���̈ʒu

    int add_attack = 2;
    bool time_up = true;
};