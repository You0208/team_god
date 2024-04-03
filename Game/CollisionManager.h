#pragma once

#include<vector>
#include<set>
#include "Character/SeedManager.h"
#include "Character/UnitManager.h"
#include "Lemur/Collision/Collision.h"

// �����蔻��}�l�[�W���[
class CollisionManager
{
private:
    CollisionManager() {}
    ~CollisionManager() {}
public:
    // �B��̃C���X�^���X�擾
    static CollisionManager& Instance()
    {
        static CollisionManager instance;
        return instance;
    }

    // ��ƃ��j�b�g�̓����蔻��
    void CollisionSeedVsUnit();

    // ��̒��n�ꏊ�̑O�Ƀ��j�b�g���������m�F�i����΋A���Ă����l�𒅒n�ꏊ�ɂ���j
    DirectX::XMFLOAT2 CollisionUnitBackVsSeed(DirectX::XMFLOAT2 position);
};