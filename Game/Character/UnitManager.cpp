#include "UnitManager.h"
#include "Lemur/Graphics/Graphics.h"
#include "Lemur/Collision/Collision.h"


// �X�V����
void UnitManager::Update(float elapsedTime)
{
    for (Unit* unit : units)
    {
        unit->Update(elapsedTime);
    }

    // �j������
    for (Unit* unit : removes)
    {
        std::vector<Unit*>::iterator it = std::find(units.begin(), units.end(), unit);
        if (it != units.end())
        {
            units.erase(it);
        }

        // �폜
        delete unit;
    }
    // �j�����X�g���N���A
    removes.clear();

}

// �`�揈��
void UnitManager::Render(float elapsedTime, ID3D11PixelShader** replaced_pixel_shader)
{
    for (Unit* unit : units)
    {
        unit->Render(elapsedTime, replaced_pixel_shader);
    }
}

void UnitManager::Register(Unit* unit)
{
    units.emplace_back(unit);
}

void UnitManager::Clear()
{
    for (Unit* unit : units)
    {
        delete unit;
    }
    units.clear();
}

void UnitManager::DrawDebugPrimitive()
{
    for (Unit* unit : units)
    {
        unit->DrawDebugPrimitive();
    }
}

// ���j�b�g���m�̏Փ˔���
void UnitManager::CollisionUnitVsSeed()
{
    UnitManager& unitManager = UnitManager::Instance();

    // �S�Ă̓G�Ƒ�������ŏՓ˔���
    int unitCount = units.size();
    for (int i = 0; i < unitCount; ++i)
    {
        Unit* unitA = units.at(i);
        for (int j = i + 1; j < unitCount; ++j)
        {
            Unit* unitB = units.at(j);
            // �Փ˔���
            DirectX::XMFLOAT3 outPosition;
            if (Collision::IntersectCylinderVsCylinder
            (unitA->GetPosition(),
                unitA->GetRadius(),
                unitA->GetHeight(),
                unitB->GetPosition(),
                unitB->GetRadius(),
                unitB->GetHeight(),
                outPosition)
                )
            {
                unitB->SetPosition(outPosition);
            }
        }
    }
}

void UnitManager::Remove(Unit* unit)
{
    // �j�����X�g�ɒǉ�
    removes.insert(unit);
}
