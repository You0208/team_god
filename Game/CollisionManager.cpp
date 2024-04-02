#include "CollisionManager.h"

void CollisionManager::CollisionSeedVsUnit()
{
    UnitManager& unitManager = UnitManager::Instance();
    SeedManager& seedtManager = SeedManager::Instance();

    // �S�Ă̓G�Ƒ�������ŏՓ˔���
    int unitCount = unitManager.GetUnitCount();
    int seedCount = seedtManager.GetSeedCount();

    bool is_intersected = false;

    // ��̑�������
    for (int j = 0; j < seedCount; ++j)
    {
        Seed* seed = seedtManager.GetSeed(j);

        is_intersected = false;
        // ���j�b�g�̑�������
        for (int i = 0; i < unitCount; ++i)
        {
            Unit* unit = unitManager.GetUnit(i);

            if (unit->category == 0 || unit->category == 3)
            {
                // �킪���j�b�g�̍U���͈͂ɓ����Ă���Ƃ�
                if (Collision::IntersectCylinderVsCylinder
                (seed->GetPosition(),
                    seed->GetRadius(),
                    seed->GetHeight(),
                    unit->GetPosition(),
                    unit->GetAttackRadius(),
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


//TODO �܂���Ԏ����ɋ߂����j�b�g���Z�o����K�v������
DirectX::XMFLOAT2 CollisionManager::CollisionUnitBackVsSeed(DirectX::XMFLOAT2 position)
{
    UnitManager& unitManager = UnitManager::Instance();

    // �S�Ă̓G�Ƒ�������ŏՓ˔���
    int unitCount = unitManager.GetUnitCount();

    // ���j�b�g�̑�������
    for (int i = 0; i < unitCount; ++i)
    {
        Unit* unit = unitManager.GetUnit(i);
        // ���j�b�g�̌��̎l�p�ɒ��n�ꏊ������΁A���j�b�g��菭����O�̍��W��Ԃ�
        if (Collision::IntersectSquareVsPoint(unit->square[0], unit->square[1], position))
        {
            return DirectX::XMFLOAT2(position.x, position.y - unit->dec_pos);
        }
    }

    return position;
}
