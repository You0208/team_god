#include "UnitManager.h"
#include "Lemur/Graphics/Graphics.h"
#include "Lemur/Collision/Collision.h"


// 更新処理
void UnitManager::Update(float elapsedTime)
{
    for (Unit* unit : units)
    {
        unit->Update(elapsedTime);
    }

    // 破棄処理
    for (Unit* unit : units)
    {
        std::vector<Unit*>::iterator it = std::find(units.begin(), units.end(), unit);
        if (it != units.end())
        {
            units.erase(it);
        }

        // 削除
        delete unit;
    }
    // 破棄リストをクリア
    removes.clear();


    // ユニット同士の当たり判定
    CollisionEnemyVsEnemis();
}

// 描画処理
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

// ユニット同士の衝突判定
void UnitManager::CollisionEnemyVsEnemis()
{
    UnitManager& unitManager = UnitManager::Instance();

    // 全ての敵と総当たりで衝突判定
    int unitCount = units.size();
    for (int i = 0; i < unitCount; ++i)
    {
        Unit* unitA = units.at(i);
        for (int j = i + 1; j < unitCount; ++j)
        {
            Unit* unitB = units.at(j);
            // 衝突判定
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
    // 破棄リストに追加
    removes.insert(unit);
}
