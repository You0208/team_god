#include "CollisionManager.h"
#include <random>

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
            if (unit->GetCategory() == 0 || unit->GetCategory() == 3)
            {
                // �킪���j�b�g�̍U���͈͂ɓ����Ă���Ƃ�
                if (Collision::IntersectCircleVsCircle
                ({ seed->GetPosition().x,seed->GetPosition().z },
                    seed->GetRadius(),
                    { unit->GetPosition().x,unit->GetPosition().z },
                    unit->GetAttackCollisionRange()
                ))
                {
                    is_intersected = true;
                    break; // ��x�ł��d�Ȃ�΃��[�v�𔲂���
                }
            }
            // �U���͈͂��O�p
            if (unit->GetCategory() == 1 || unit->GetCategory() == 2)
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
            // �U���͈͂��l�p
            if (unit->GetCategory() == 4 || unit->GetCategory() == 5)
            {
                // �킪���j�b�g�̍U���͈͂ɓ����Ă���Ƃ�
                if (Collision::IntersectRectVsCircle
                (
                    unit->GetAttackRect(),
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
        if (!is_intersected)
        {
            seed->SetBorn(true);
            break;
        }
        else seed->SetBorn(false);
       
    }
}


// TODO ������
DirectX::XMFLOAT2 CollisionManager::CollisionSeedVsSeed(DirectX::XMFLOAT2 position)
{
    // �S�Ă̎�Ƒ�������ŏՓ˔���
    SeedManager& seedtManager = SeedManager::Instance();
    int seedCount = seedtManager.GetSeedCount();
    DirectX::XMFLOAT2 s_p = {};
    if (seedCount < 2)
    {
        return position;
    }
    // ��̑�������
    for (int i = 0; i < seedCount; ++i)
    {
        Seed* seed = seedtManager.GetSeed(i);
        s_p = { seed->GetPosition().x,seed->GetPosition().z };
        // ���n�n�_����̒��ɂ���
        if (Collision::IntersectCircleVsPosition(position, s_p, seed->GetRadius()))
        {
            //std::mt19937 mt{ std::random_device{}() };
            //std::uniform_int_distribution<float> width(-1.0f, 1.0f);
            //std::uniform_int_distribution<float> height(0.0f, -1.0f);           
            //DirectX::XMFLOAT2 out_direction = { float(width(mt)),float(height(mt)) };

            float x = (rand() % 20 - 10) * 0.1f;
            float y = (rand() % 10) * -0.1f;

            DirectX::XMFLOAT2 out_direction = { x,y };
            if (0.01f >= out_direction.x && -0.01f >= out_direction.x)  out_direction.x = 1.0f;
            if (0.01f >= out_direction.y && -0.01f >= out_direction.y)  out_direction.y = -1.0f;

            out_direction = Normalize(out_direction);

            return { seed->GetPosition().x + out_direction.x * seed->GetRadius() * 2, seed->GetPosition().z + out_direction.y * seed->GetRadius() * 2 };
        }
    }

    return position;
}

bool CollisionManager::CollisionUnitBackVsSeed_Re(DirectX::XMFLOAT2 position)
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

        // ���j�b�g�ɒ��ړ������Ă�����
        if (Collision::IntersectCircleVsPosition(position, { unit->GetPosition().x,unit->GetPosition().z }, unit->GetRadius()))
        {
            return true;
        }
        // ���n�n�_���l�p�ɓ����Ă���Ƃ�
        if (Collision::IntersectSquareVsPoint(unit->GetRect().left_up, unit->GetRect().right_down, position))
        {
            index.push_back(i);// �ԍ����L�^
        }
    }

    // �Y�����j�b�g���Ȃ���ΏI��
    if (index.size() == 0)  return false;

    // �L�^�����ԍ��̃��j�b�g�𓖂���
    for (int j = 0; j < index.size(); ++j)
    {
        unit = unitManager.GetUnit(index.at(j));

        if (j == 0)// �n�߂͔�r�Ȃ�
        {
            near_pos = unit->GetPosition().z;
            break;
        }
        else if (unit->GetPosition().z < near_pos)
        {
            near_pos = unit->GetPosition().z;
            near_index = j;
        }
    }

    unit = unitManager.GetUnit(index.at(near_index));
    if (Collision::IntersectSquareVsPoint(unit->GetRect().left_up, unit->GetRect().right_down, position))
    {
        return true;
    }

}

bool CollisionManager::CollisionUnitBackVsSeed_Re(DirectX::XMFLOAT2 position, DirectX::XMFLOAT2& dis_pos)
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
        if (Collision::IntersectSquareVsPoint(unit->GetRect().left_up, unit->GetRect().right_down, position))
        {
            index.push_back(i);// �ԍ����L�^
        }
    }

    // �Y�����j�b�g���Ȃ���ΏI��
    if (index.size() == 0)  return false;

    // �L�^�����ԍ��̃��j�b�g�𓖂���i�킪�͈͂ɐڐG�������j�b�g�̑�������j
    for (int j = 0; j < index.size(); ++j)
    {
        unit = unitManager.GetUnit(index.at(j));

        if (j == 0)// �n�߂͔�r�Ȃ�
        {
            near_pos = unit->GetPosition().z;
        }
        else if (unit->GetPosition().z < near_pos)
        {
            near_pos = unit->GetPosition().z;
            near_index = j;
        }
    }

    unit = unitManager.GetUnit(index.at(near_index));
    if (Collision::IntersectSquareVsPoint(unit->GetRect().left_up, unit->GetRect().right_down, position))
    {
        dis_pos = { unit->GetPosition().x,unit->GetPosition().z };
        return true;
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
        if (Collision::IntersectSquareVsPoint(unit->GetRect().left_up, unit->GetRect().right_down, position))
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
            near_pos = unit->GetPosition().z;
            break;
        }
        else if (unit->GetPosition().z < near_pos)
        {
            near_pos = unit->GetPosition().z;
            near_index = j;
        }
    }

    unit = unitManager.GetUnit(index.at(near_index));
    if (Collision::IntersectSquareVsPoint(unit->GetRect().left_up, unit->GetRect().right_down, position))
    {
        return DirectX::XMFLOAT2(position.x, unit->GetPosition().z - unit->GetDecPos());
    }

    return position;
}
