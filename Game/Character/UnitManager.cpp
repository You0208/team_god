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

    for (int i = 0; i < GetUnitCount(); i++)
    {
        Unit* unit = units.at(i);
        unit->DrawDebugGUI(i);
    }

    // 破棄処理
    for (Unit* unit : removes)
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

}

// 描画処理
void UnitManager::Render(float elapsedTime, ID3D11PixelShader** replaced_pixel_shader)
{
    DrawDebugPrimitive();
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

void UnitManager::Remove(Unit* unit)
{
    // 破棄リストに追加
    removes.insert(unit);
}
