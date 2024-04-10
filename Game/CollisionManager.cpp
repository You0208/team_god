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

            // �U���͈͂��~
            if (unit->category == 0 || unit->category == 3)
            {
                // �킪���j�b�g�̍U���͈͂ɓ����Ă���Ƃ�
                if (Collision::IntersectCircleVsCircle
                ({ seed->GetPosition().x,seed->GetPosition().z },
                    seed->GetRadius(),
                    { unit->GetPosition().x,unit->GetPosition().z },
                    unit->GetAttackRadius()
                ))
                {
                    is_intersected = true;
                    break; // ��x�ł��d�Ȃ�΃��[�v�𔲂���
                }
            }
            // �U���͈͂��O�p
            if (unit->category == 1 || unit->category == 2)
            {
                // �킪���j�b�g�̍U���͈͂ɓ����Ă���Ƃ�
                if (Collision::IntersectTriangleVsCircle
                (
                    unit->GetTriangle1(),
                    { seed->GetPosition().x,seed->GetPosition().z },
                    seed->GetRadius()
                    ))
                {
                    is_intersected = true;
                    break; // ��x�ł��d�Ȃ�΃��[�v�𔲂���
                }
                // �킪���j�b�g�̍U���͈͂ɓ����Ă���Ƃ�
                if (Collision::IntersectTriangleVsCircle
                (
                    unit->GetTriangle2(),
                    { seed->GetPosition().x,seed->GetPosition().z },
                    seed->GetRadius()
                ))
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

DirectX::XMFLOAT2 CollisionManager::CollisionUnitBackVsSeed(DirectX::XMFLOAT2 position)
{
    UnitManager& unitManager = UnitManager::Instance();

    // ���n�ꏊ��艜�A����ԋ����̋߂����j�b�g��T��
    int unitCount = unitManager.GetUnitCount();
  
    Unit* unit = nullptr;   
    std::vector<int> index; // ���n�_�ɓ����Ă郆�j�b�g�ԍ�
    int near_index = 0;     // ��O�̃��j�b�g�ԍ�
    float near_pos = 0.0f;  // ��Ԏ�O�̍��W

    // ���j�b�g��������
    for (int i = 0; i < unitCount; ++i)
    {
        unit = unitManager.GetUnit(i);

        // ���n�n�_���l�p�ɓ����Ă���Ƃ�
        if (Collision::IntersectSquareVsPoint(unit->square.left_up, unit->square.right_down, position))
        {
            index.push_back(i);// �ԍ����L�^
        }
    }

    // �Y�����j�b�g���Ȃ���ΏI��
    if (index.size() == 0)  return position;

    // �L�^�����ԍ��̃��j�b�g�𓖂���
    for (int j = 0; j < index.size(); ++j)
    {
        unit = unitManager.GetUnit(index.at(j));

        if (j == 0)// �n�߂͔�r�Ȃ�
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
    if (Collision::IntersectSquareVsPoint(unit->square.left_up, unit->square.right_down, position))
    {
        return DirectX::XMFLOAT2(position.x, unit->position.z - unit->dec_pos);
    }

    return position;
}
