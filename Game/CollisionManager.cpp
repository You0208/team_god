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

    // ���n�ꏊ��艜�A����ԋ����̋߂����j�b�g��T��
    int unitCount = unitManager.GetUnitCount();
    //int near_index = 0;  // ��ԋ߂����j�b�g�ԍ�
    //float dis_small = 0.0f; //��ԒZ������
    //
    //Unit* unit = nullptr;

    //// ���j�b�g��������
    //for (int i = 0; i < unitCount; ++i)
    //{
    //    unit = unitManager.GetUnit(i);

    //    if (unit->position.z >= position.y)//���n�ꏊ��艜
    //    {
    //        float d = sqrtf(unit->position.x - position.x * unit->position.x - position.x + unit->position.z - position.y * unit->position.z - position.y);
    //        if (i == 0)// �n�߂̃��j�b�g�͑���̂�
    //        {
    //            dis_small = d;
    //            break;
    //        }
    //        if (dis_small > d)// ��ԏ������������X�V����
    //        {
    //            dis_small = d;
    //            near_index = i;
    //        }
    //    }
    //}

    //// ��ԋ߂����j�b�g�̌��ɓ����Ă��Ȃ����ǂ����m�F�i�����Ă���΃��j�b�g�̍��W���O�̍��W��Ԃ��j
    //if (unitCount > 0)
    //{
    //    unit = unitManager.GetUnit(near_index);
    //    if (Collision::IntersectSquareVsPoint(unit->square[0], unit->square[1], position))
    //    {
    //        return DirectX::XMFLOAT2(position.x, unit->position.z - unit->dec_pos);
    //    }
    //}
    //return position;

    //// ���j�b�g�̑�������
    //for (int i = 0; i < unitCount; ++i)
    //{
    //    Unit* unit = unitManager.GetUnit(i);
    //    // ���j�b�g�̌��̎l�p�ɒ��n�ꏊ������΁A���j�b�g��菭����O�̍��W��Ԃ�
    //    if (Collision::IntersectSquareVsPoint(unit->square[0], unit->square[1], position))
    //    {
    //        return DirectX::XMFLOAT2(position.x, unit->position.z - unit->dec_pos);
    //    }
    //}

    //// ���n�n�_�Ɏl�p�����Ԃ��Ă��郆�j�b�g���̒��ŁA��Ԏ�O�ɂ�����̂�T��
    //int near_index = 0;  // ��O�̃��j�b�g�ԍ�
    //float near_pos = 0.0f;
    //Unit* unit = nullptr;

    //// ���j�b�g��������
    //for (int i = 0; i < unitCount; ++i)
    //{
    //    unit = unitManager.GetUnit(i);

    //    if (Collision::IntersectSquareVsPoint(unit->square[0], unit->square[1], position))
    //    {
    //        if (unit->position.z >= position.y)//���n�ꏊ��艜
    //        {
    //            if (i == 0)
    //            {
    //                near_pos = unit->position.z;
    //                break;
    //            }
    //            if (unit->position.z < near_pos)
    //            {
    //                near_pos = unit->position.z;
    //                near_index = i;
    //            }
    //        }
    //    }
    //}

    //// ��ԋ߂����j�b�g�̌��ɓ����Ă��Ȃ����ǂ����m�F�i�����Ă���΃��j�b�g�̍��W���O�̍��W��Ԃ��j
    //if (unitCount > 0)
    //{
    //    unit = unitManager.GetUnit(near_index);
    //    if (Collision::IntersectSquareVsPoint(unit->square[0], unit->square[1], position))
    //    {
    //        return DirectX::XMFLOAT2(position.x, unit->position.z - unit->dec_pos);
    //    }
    //}
    
    Unit* unit = nullptr;
    std::vector<int> index;
    int near_index = 0;  // ��O�̃��j�b�g�ԍ�
    float near_pos = 0.0f;
    // ���j�b�g��������
    for (int i = 0; i < unitCount; ++i)
    {
        unit = unitManager.GetUnit(i);

        // ���n�n�_���l�p�ɓ����Ă���Ƃ�
        if (Collision::IntersectSquareVsPoint(unit->square[0], unit->square[1], position))
        {
            index.push_back(i);// �ԍ����L�^
        }

    }

    if (index.size() == 0)  return position;

    // �L�^�����ԍ��̃��j�b�g�𓖂���
    for (int j = 0; j < index.size(); ++j)
    {
        unit = unitManager.GetUnit(index.at(j));

        if (j == 0)
        {
            near_pos = unit->position.z;
            break;
        }
        else if (unit->position.z < near_pos)
        {
            near_pos = unit->position.z;
            near_index = j;
        }
    }

    unit = unitManager.GetUnit(index.at(near_index));
    if (Collision::IntersectSquareVsPoint(unit->square[0], unit->square[1], position))
    {
        return DirectX::XMFLOAT2(position.x, unit->position.z - unit->dec_pos);
    }

    return position;
}
