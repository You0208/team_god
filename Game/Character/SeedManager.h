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

    // ��o�^
    void Register(Seed* enemy);

    // ��S�폜
    void Clear();

    //�f�o�b�O�v���~�e�B�u�`��
    void DrawDebugPrimitive();

    // �퐔�擾
    int GetSeedCount() const { return static_cast<int>(seeds.size()); }

    // ��擾
    Seed* GetSeed(int index) { return seeds.at(index); }

    // ��폜
    void Remove(Seed* enemy);

    // ��ƃ��j�b�g�̓����蔻��
    void SeedVsUnit();

    // ��ƍ�̓����蔻��
    void SeedVsFence();

    int seed_number = 0;// ���̎�ԍ�
private:
    std::vector<Seed*> seeds;
    std::set<Seed*>    removes;

    float seed_limit_time = 15.0f;// ��̐�������
};