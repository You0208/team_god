#include "SeedManager.h"
#include "Lemur/Graphics/Graphics.h"
#include "Lemur/Collision/Collision.h"

// �X�V����
void SeedManager::Update(float elapsedTime)
{
    // ���Ԑ؂�̎�������iseed->death��true�̂Ƃ��j
    seeds.erase(std::remove_if(seeds.begin(), seeds.end(), [](Seed* seed) {return seed->death; }), seeds.end());

    for (Seed* seed : seeds)
    {
        // ���Ԑ؂�̎�
        if (seed->GetTimer() >= seed_limit_time)
        {
            // ���S�ɐ؂�ւ�
            seed->death = true;

        }
        else
        {
            // ��̍X�V
            seed->Update(elapsedTime);
        }

        // ���S���̏���
        if (seed->death)
        {
            for (Seed* s : seeds)
            {
                // �����폜�\��̎�ԍ������傫���ԍ��̎킪����΁A�ԍ������Z����
                if (seed->GetNumber() < s->GetNumber())s->DecNumber();
            }
            // ���݂̎�ԍ������Z
            seed_number--;
        }
    }

    // �j������
    for (Seed* seed : removes)
    {
        std::vector<Seed*>::iterator it = std::find(seeds.begin(), seeds.end(), seed);
        if (it != seeds.end())
        {
            seeds.erase(it);
        }

        // �폜
        delete seed;
    }
    // �j�����X�g���N���A
    removes.clear();
}

// �`�揈��
void SeedManager::Render(float elapsedTime, ID3D11PixelShader** replaced_pixel_shader)
{
    for (Seed* seed : seeds)
    {
        seed->Render(elapsedTime, replaced_pixel_shader);
    }
}

void SeedManager::Register(Seed* seed)
{
    // �����l��ݒ�
    seed->SetNumber(seed_number);

    seeds.emplace_back(seed);
    // �ԍ������Z
    seed_number++;
}

void SeedManager::Clear()
{
    for (Seed* seed : seeds)
    {
        delete seed;
    }
    seeds.clear();
}

void SeedManager::DrawDebugPrimitive()
{
    for (Seed* seed : seeds)
    {
        seed->DrawDebugPrimitive();
    }
}

// �G�l�~�[���m�̏Փ˔���
void SeedManager::CollisionSeedVsSeeds()
{
    SeedManager& seedManager = SeedManager::Instance();

    // �S�Ă̓G�Ƒ�������ŏՓ˔���
    int seedCount = seeds.size();
    for (int i = 0; i < seedCount; ++i)
    {
        Seed* seedA = seeds.at(i);
        for (int j = i + 1; j < seedCount; ++j)
        {
            Seed* seedB = seeds.at(j);
            // �Փ˔���
            DirectX::XMFLOAT3 outPosition;
            if (Collision::IntersectCylinderVsCylinder
            (seedA->GetPosition(),
                seedA->GetRadius(),
                seedA->GetHeight(),
                seedB->GetPosition(),
                seedB->GetRadius(),
                seedB->GetHeight(),
                outPosition)
                )
            {
                seedB->SetPosition(outPosition);
            }
        }
    }
}

void SeedManager::Remove(Seed* seed)
{
    // �j�����X�g�ɒǉ�
    removes.insert(seed);
}
