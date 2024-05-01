#include "UnitManager.h"
#include "Lemur/Graphics/Graphics.h"
#include "Lemur/Collision/Collision.h"

void UnitManager::Initialize()
{
    //TODO ‚à‚Ë@ƒ†ƒjƒbƒgƒXƒe[ƒ^ƒX
    {
        //Unit_A (ŠÛ)
        unit_A.attack_interval = 1.0f;    // UŒ‚ŠÔŠu
        unit_A.attack_power    = 3;       // UŒ‚—Í
        unit_A.attack_times    = 1;       // UŒ‚‰ñ”
        radius_A               = 1.5f;    // UŒ‚”¼Œa

        //Unit_BiOŠp‰¡j
        unit_B.attack_interval = 1.0f;  // UŒ‚ŠÔŠu
        unit_B.attack_power = 4;        // UŒ‚—Í
        unit_B.attack_times = 1;        // UŒ‚‰ñ”
        t_height_B = 1.59f;             // OŠpŒ`‚Ì‚‚³
        t_base_B = 1.59f;               // OŠpŒ`‚Ì’ê•Ó’·

        //Unit_CiOŠpcj
        unit_C.attack_interval = 1.0f;  // UŒ‚ŠÔŠu
        unit_C.attack_power = 4;        // UŒ‚—Í
        unit_C.attack_times = 1;        // UŒ‚‰ñ”
        t_height_C = 1.59f;              // OŠpŒ`‚Ì‚‚³
        t_base_C = 1.59f;                // OŠpŒ`‚Ì’ê•Ó’·

        //Unit_Diƒoƒtj
        radius_D               = 3.0f; // UŒ‚”¼Œa
        timer_max_D            = 5.0f; // oŒ»ŠÔ
        streng_width_D         = 2;    // UŒ‚—Í‚Ìã‚è•

        //Unit_Ei’¼ü‰¡j
        unit_E.attack_interval = 1.0f;  // UŒ‚ŠÔŠu
        unit_E.attack_power    = 1;     // UŒ‚—Í
        unit_E.attack_times    = 1;     // UŒ‚‰ñ”
        attack_width_E         = 0.9f;  // UŒ‚”ÍˆÍ‚Ì•

        //Unit_Fi’¼ücj
        unit_F.attack_interval = 1.0f; // UŒ‚ŠÔŠu
        unit_F.attack_power    = 1;    // UŒ‚—Í
        unit_F.attack_times    = 1;    // UŒ‚‰ñ”
        attack_width_F         = 0.9f; // UŒ‚”ÍˆÍ‚Ì•

        //Unit_Hi‰EÎ‚ßj
        unit_H.attack_interval = 1.0f;        // UŒ‚ŠÔŠu
        unit_H.attack_power    = 1;           // UŒ‚—Í
        unit_H.attack_times    = 1;           // UŒ‚‰ñ”
        attack_width_H         = {1.5f,20.0f};// UŒ‚”ÍˆÍ‚Ì•
        rect_angle_H           = 50.0f;       // UŒ‚”ÍˆÍ‚Ì•

        //Unit_Ii¶Î‚ßj
        unit_I.attack_interval = 1.0f;          // UŒ‚ŠÔŠu
        unit_I.attack_power    = 1;             // UŒ‚—Í
        unit_I.attack_times    = 1;             // UŒ‚‰ñ”
        attack_width_I         = { 1.5f,20.0f };// UŒ‚”ÍˆÍ‚Ì•
        rect_angle_I           = -50.0f;        // UŒ‚”ÍˆÍ‚Ì•

        //Unit_J(ƒh[ƒiƒc)
        unit_J.attack_interval = 1.0f; // UŒ‚ŠÔŠu
        unit_J.attack_power    = 1;    // UŒ‚—Í
        unit_J.attack_times    = 1;    // UŒ‚‰ñ”
        radius_in_J            =1.0f;  // ”¼Œa
        radius_out_J           =1.7f;  // ”¼Œa

    }
}

// XVˆ—
void UnitManager::Update(float elapsedTime)
{
    DebugImGui();

    for (Unit* unit : units)
    {
        unit->Update(elapsedTime);
    }

    for (int i = 0; i < GetUnitCount(); i++)
    {
        Unit* unit = units.at(i);
        unit->DrawDebugGUI(i);
    }

    // ”jŠüˆ—
    for (Unit* unit : removes)
    {
        std::vector<Unit*>::iterator it = std::find(units.begin(), units.end(), unit);
        if (it != units.end())
        {
            units.erase(it);
        }

        // íœ
        delete unit;
    }
    // ”jŠüƒŠƒXƒg‚ğƒNƒŠƒA
    removes.clear();

}

// •`‰æˆ—
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
    // ”jŠüƒŠƒXƒg‚É’Ç‰Á
    removes.insert(unit);
}

void UnitManager::DebugImGui()
{
    ImGui::Begin(u8"ƒ†ƒjƒbƒg");
    if (ImGui::TreeNode("unit1"))
    {
        UnitImGui(unit_A);
        ImGui::SliderFloat(u8"UŒ‚”¼Œa", &radius_A, 0.0f, 5.0f);
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("unit2"))
    {
        UnitImGui(unit_B);
        ImGui::SliderFloat(u8"OŠpŒ`‚Ì‚‚³", &t_height_B, 0.0f, 5.0f);
        ImGui::SliderFloat(u8"OŠpŒ`‚Ì’ê•Ó’·", &t_base_B, 0.0f, 5.0f);
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("unit3"))
    {
        UnitImGui(unit_C);
        ImGui::SliderFloat(u8"OŠpŒ`‚Ì‚‚³", &t_height_C, 0.0f, 5.0f);
        ImGui::SliderFloat(u8"OŠpŒ`‚Ì’ê•Ó’·", &t_base_C, 0.0f, 5.0f);
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("unit4"))
    {
        ImGui::SliderFloat(u8"UŒ‚”¼Œa", &radius_D, 0.0f, 5.0f);
        ImGui::SliderFloat(u8"oŒ»ŠÔ", &timer_max_D, 0.0f, 10.0f);
        ImGui::SliderInt(u8"ƒoƒt‚·‚éUŒ‚—Íiƒvƒ‰ƒX‚Ì•ªj", &streng_width_D, 1, 5);
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("unit5"))
    {
        UnitImGui(unit_E);
        ImGui::SliderFloat(u8"UŒ‚•", &attack_width_E, 0.1f, 10.0f);
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("unit6"))
    {
        UnitImGui(unit_F);
        ImGui::SliderFloat(u8"UŒ‚•", &attack_width_F, 0.1f, 10.0f);
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("unit7"))
    {
        UnitImGui(unit_H);
        ImGui::SliderFloat2(u8"UŒ‚•", &attack_width_H.x, 0.1f, 20.0f);
        ImGui::SliderFloat(u8"UŒ‚Šp“x", &rect_angle_H, 0.0f, 360.0f);
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("unit8"))
    {
        UnitImGui(unit_I);
        ImGui::SliderFloat2(u8"UŒ‚•", &attack_width_I.x, 0.1f, 20.0f);
        ImGui::SliderFloat(u8"UŒ‚Šp“x", &rect_angle_I, 0.0f, 360.0f);
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("unit9"))
    {
        UnitImGui(unit_J);
        ImGui::SliderFloat(u8"ƒh[ƒiƒc’†”¼Œa", &radius_in_J, 0.0f, 5.0f);
        ImGui::SliderFloat(u8"ƒh[ƒiƒcŠO”¼Œa", &radius_out_J, 0.0f, 5.0f);
        ImGui::TreePop();
    }
    ImGui::End();
}

void UnitManager::SetUpUnit_A(Unit* unit)
{
    unit->SetAttackTimes(unit_A.attack_times);
    unit->SetAttackPower(unit_A.attack_power);
    unit->SetAttackInterval(unit_A.attack_interval);
    unit->SetAttackRadius(radius_A);
}

void UnitManager::SetUpUnit_B(Unit* unit)
{
    unit->SetAttackTimes(unit_B.attack_times);
    unit->SetAttackPower(unit_B.attack_power);
    unit->SetAttackInterval(unit_B.attack_interval);
    unit->SetTHeight(t_height_B);
    unit->SetTBase(t_base_B);
}

void UnitManager::SetUpUnit_C(Unit* unit)
{
    unit->SetAttackTimes(unit_C.attack_times);
    unit->SetAttackPower(unit_C.attack_power);
    unit->SetAttackInterval(unit_C.attack_interval);
    unit->SetTHeight(t_height_C);
    unit->SetTBase(t_base_C);
}

void UnitManager::SetUpUnit_D(Unit* unit)
{
    unit->SetAttackRadius(radius_D);
    unit->SetTimerMax(timer_max_D);
    unit->SetStrengWidth(streng_width_D);
}

void UnitManager::SetUpUnit_E(Unit* unit)
{
    unit->SetAttackTimes(unit_E.attack_times);
    unit->SetAttackPower(unit_E.attack_power);
    unit->SetAttackInterval(unit_E.attack_interval);
    unit->SetAttackWidth(attack_width_E);
}

void UnitManager::SetUpUnit_F(Unit* unit)
{
    unit->SetAttackTimes(unit_F.attack_times);
    unit->SetAttackPower(unit_F.attack_power);
    unit->SetAttackInterval(unit_F.attack_interval);
    unit->SetAttackWidth(attack_width_F);
}

void UnitManager::SetUpUnit_H(Unit* unit)
{
    unit->SetAttackTimes(unit_H.attack_times);
    unit->SetAttackPower(unit_H.attack_power);
    unit->SetAttackInterval(unit_H.attack_interval);
    unit->SetAttackWidth(attack_width_H);
    unit->SetRectAngle(rect_angle_H);
}

void UnitManager::SetUpUnit_I(Unit* unit)
{
    unit->SetAttackTimes(unit_I.attack_times);
    unit->SetAttackPower(unit_I.attack_power);
    unit->SetAttackInterval(unit_I.attack_interval);
    unit->SetAttackWidth(attack_width_I);
    unit->SetRectAngle(rect_angle_I);
}

void UnitManager::SetUpUnit_J(Unit* unit)
{
    unit->SetAttackTimes(unit_J.attack_times);
    unit->SetAttackPower(unit_J.attack_power);
    unit->SetAttackInterval(unit_J.attack_interval);
    unit->SetAttackRadius(radius_in_J);
    unit->SetAttackRadius(radius_out_J);
}
