#include "SeedManager.h"
#include "UnitManager.h"
#include "Game/Stage/Fence.h"
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

    // ��VS���j�b�g
    SeedVsUnit();
    // ��VS��
    SeedVsFence();
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

void SeedManager::SeedVsUnit()
{
    SeedManager& seedManager = SeedManager::Instance();
    UnitManager& unitManager = UnitManager::Instance();

    // �S�Ă̎�Ƒ�������ŏՓ˔���
    int seedCount = seedManager.GetSeedCount();
    int unitCount = unitManager.GetUnitCount();

    for (int i = 0; i < seedCount; ++i)
    {
        Seed* seed = seedManager.GetSeed(i);
        for (int j = 0; j < unitCount; ++j)
        {
            Unit* unit = unitManager.GetUnit(j);

            // ���j�b�g�Ǝ킪�������Ă��������
            if (Collision::IntersectCircleVsCircle(
                { seed->GetPosition().x, seed->GetPosition().z },
                seed->GetRadius(),
                { unit->GetPosition().x, unit->GetPosition().z },
                unit->GetRadius()
            ))
            {
                seed->SetDead(true);
            }
        }
    }

}

void SeedManager::SeedVsFence()
{
    SeedManager& seedManager = SeedManager::Instance();
    // �S�Ă̎�Ƒ�������ŏՓ˔���
    int seedCount = seedManager.GetSeedCount();

    for (int i = 0; i < seedCount; ++i)
    {
        Seed* seed = seedManager.GetSeed(i);
        // ��Ǝ킪�������Ă��������
        if (Collision::IntersectCircleVsCircle(
            { seed->GetPosition().x, seed->GetPosition().z },
            seed->GetRadius(),
            { Fence::Instance().GetPosition().x, Fence::Instance().GetPosition().z },
            Fence::Instance().GetRadius()
        ))
        {
            seed->SetDead(true);
        }
    }

}
