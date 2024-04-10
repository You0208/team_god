#include "SeedManager.h"
#include "UnitManager.h"
#include "Lemur/Graphics/Graphics.h"
#include "Lemur/Collision/Collision.h"
#include "Game/CollisionManager.h"

// �X�V����
void SeedManager::Update(float elapsedTime)
{
    for (Seed* seed : seeds)
    {
        // ���Ԑ؂�̎�
        if (seed->GetTimer() >= seed_limit_time)
        {
            // ���S�ɐ؂�ւ�
            seed->SetDead(true);
        }
        else
        {
            // ��̍X�V
            seed->Update(elapsedTime);
        }

        // ���S���̏���
        if (seed->IsDead())
        {
            for (Seed* s : seeds)
            {
                // �����폜�\��̎�ԍ������傫���ԍ��̎킪����΁A�ԍ������Z����
                if (seed->GetNumber() < s->GetNumber())s->DecNumber();
            }
            // ���݂̎�ԍ������Z
            seed_number--;

            // ������X�g�������
            Remove(seed);
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
    DrawDebugPrimitive();
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

void SeedManager::Remove(Seed* seed)
{
    // �j�����X�g�ɒǉ�
    removes.insert(seed);
}
