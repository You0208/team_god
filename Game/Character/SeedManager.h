#pragma once

#include<vector>
#include<set>
#include "Seed.h"

// ��}�l�[�W���[
class SeedManager
{
private:
    SeedManager() {}
    ~SeedManager() {}
public:
    // �B��̃C���X�^���X�擾
    static SeedManager& Instance()
    {
        static SeedManager instance;
        return instance;
    }

    // �X�V����
    void Update(float elapsedTime);

    // �`�揈��
    void Render(float elapsedTime, ID3D11PixelShader** replaced_pixel_shader);

    // �G�l�~�[�o�^
    void Register(Seed* enemy);

    // �G�l�~�[�S�폜
    void Clear();

    //�f�o�b�O�v���~�e�B�u�`��
    void DrawDebugPrimitive();

    // �G�l�~�[���擾
    int GetEnemyCount() const { return static_cast<int>(seeds.size()); }

    // �G�l�~�[�擾
    Seed* GetEnemy(int index) { return seeds.at(index); }

    // �G�l�~�[���m�̏Փ˔���
    void CollisionSeedVsSeeds();

    // �G�l�~�[�폜
    void Remove(Seed* enemy);

private:
    std::vector<Seed*> seeds;
    std::set<Seed*>    removes;

    float seed_limit_time = 15.0f;// ��̐�������
    int seed_number = 0;// ���̎�ԍ�
};