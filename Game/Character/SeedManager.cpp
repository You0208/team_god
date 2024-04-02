#include "SeedManager.h"
#include "UnitManager.h"
#include "Lemur/Graphics/Graphics.h"
#include "Lemur/Collision/Collision.h"

// �X�V����
void SeedManager::Update(float elapsedTime)
{
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

            // ������X�g�������
            Remove(seed);
        }
    }

    CollisionSeedVsUnit();

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

// ��ƃ��j�b�g�̓����蔻��
void SeedManager::CollisionSeedVsUnit()
{
    UnitManager& unitManager = UnitManager::Instance();

    // �S�Ă̓G�Ƒ�������ŏՓ˔���
    int unitCount = unitManager.GetUnitCount();
    bool is_intersected =false;

    // ��̑�������
    for (Seed* seed : seeds)
    {
        is_intersected = false;
        // ���j�b�g�̑�������
        for (int i = 0; i < unitCount; ++i)
        {
            Unit* unit = unitManager.GetUnit(i);

            if (unit->category==0||unit->category==3)
            {
                // �킪���j�b�g�͈̔͂ɓ����Ă���Ƃ�
                if (Collision::IntersectCylinderVsCylinder
                (seed->GetPosition(),
                    seed->GetRadius(),
                    seed->GetHeight(),
                    unit->GetPosition(),
                    unit->GetRadius(),
                    unit->GetHeight())
                    )
                {
                    is_intersected = true;
                    break; // ��x�ł��d�Ȃ�΃��[�v�𔲂���
                }
            }
        }

        // �d�Ȃ肪�Ȃ��ꍇ�A��𐶂܂ꂽ��Ԃɐݒ�
        if (!is_intersected)seed->SetBorn(true);
    }
}

void SeedManager::Remove(Seed* seed)
{
    // �j�����X�g�ɒǉ�
    removes.insert(seed);
}
