#include "UnitManager.h"
#include "Lemur/Graphics/Graphics.h"
#include "Lemur/Collision/Collision.h"

void UnitManager::Initialize()
{
    //TODO ŕË@jbgXe[^X
    {
        //Chili
        InitializeBasic(unit_status[UNIT_INDEX::Chili],
            0.0f,// UÔu
            3,   // UÍ
            1,   // Uń
            0.3f,// UGtFNgTCY
            0.3f,// SGtFNgTCY
            0.3f // ÝuGtFNgTCY
        );
        unit_status[UNIT_INDEX::Chili].attack_radius                  = 1.60f;                 // Uźa

        //Shishito
        InitializeBasic(unit_status[UNIT_INDEX::Shishito],
            0.0f,// UÔu
            0,   // UÍ
            0,   // Uń
            0.25f,// UGtFNgTCY
            0.3f,// SGtFNgTCY
            0.3f // ÝuGtFNgTCY
        );
        unit_status[UNIT_INDEX::Shishito].attack_radius = 3.0f;                 // Uźa
        unit_status[UNIT_INDEX::Shishito].timer_max = 7.0f;                 // oťÔ
        unit_status[UNIT_INDEX::Shishito].streng_width = 2;                    // UÍĚăč

        //OrangePumpkin
        InitializeBasic(unit_status[UNIT_INDEX::OrangePumpkin],
            0.0f,// UÔu
            5,   // UÍR
            1,   // Uń
            0.3f,// UGtFNgTCY
            0.3f,// SGtFNgTCY
            0.3f // ÝuGtFNgTCY
        );
        unit_status[UNIT_INDEX::OrangePumpkin].t_height = 1.7f;             // Op`Ěł
        unit_status[UNIT_INDEX::OrangePumpkin].t_base = 1.59f;             // Op`ĚęÓˇ

        //GreenPumpkin
        InitializeBasic(unit_status[UNIT_INDEX::GreenPumpkin],
            0.0f,// UÔu
            5,   // UÍ
            1,   // Uń
            0.1f,// UGtFNgTCY
            0.3f,// SGtFNgTCY
            0.3f // ÝuGtFNgTCY
        );
        unit_status[UNIT_INDEX::GreenPumpkin].t_height = 1.7f;                // Op`Ěł
        unit_status[UNIT_INDEX::GreenPumpkin].t_base   = 1.59f;                // Op`ĚęÓˇ

        //Broccoli
        InitializeBasic(unit_status[UNIT_INDEX::Broccoli],
            0.0f,// UÔu
            1,   // UÍ
            1,   // Uń
            0.5f,// UGtFNgTCY
            0.3f,// SGtFNgTCY
            0.3f // ÝuGtFNgTCY
        );
        unit_status[UNIT_INDEX::Broccoli].attack_width            = 0.9f;                 // UÍÍĚ

        //Cauliflower
        InitializeBasic(unit_status[UNIT_INDEX::Cauliflower],
            0.0f,// UÔu
            1,   // UÍ
            1,   // Uń
            0.5f,// UGtFNgTCY
            0.3f,// SGtFNgTCY
            0.3f // ÝuGtFNgTCY
        );
        unit_status[UNIT_INDEX::Cauliflower].attack_width = 0.9f;                 // UÍÍĚ

        //Unit_J
        InitializeBasic(unit_status[UNIT_INDEX::J],
            0.0f,// UÔu
            3,   // UÍ
            1,   // Uń
            0.5f,// UGtFNgTCY
            0.3f,// SGtFNgTCY
            0.3f // ÝuGtFNgTCY
        );               
        unit_status[UNIT_INDEX::J].radius_in =0.9f;                  // źa
        unit_status[UNIT_INDEX::J].attack_radius =1.6f;                  // źa
    }
}

// XV
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

    // jü
    for (Unit* unit : removes)
    {
        std::vector<Unit*>::iterator it = std::find(units.begin(), units.end(), unit);
        if (it != units.end())
        {
            units.erase(it);
        }

        // í
        delete unit;
    }
    // jüXgđNA
    removes.clear();

}

// `ć
void UnitManager::Render(float elapsedTime, ID3D11PixelShader* replaced_pixel_shader)
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
    // jüXgÉÇÁ
    removes.insert(unit);
}

void UnitManager::DebugImGui()
{
    ImGui::Begin(u8"jbg");
    if (ImGui::TreeNode("chili"))
    {
        UnitImGui(unit_status[UNIT_INDEX::Chili]);
        ImGui::SliderFloat(u8"Uźa", &unit_status[UNIT_INDEX::Chili].attack_radius, 0.0f, 5.0f);
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("shishito"))
    {
        ImGui::SliderFloat(u8"Uźa", &unit_status[UNIT_INDEX::Shishito].attack_radius, 0.0f, 5.0f);
        ImGui::SliderFloat(u8"oťÔ", &unit_status[UNIT_INDEX::Shishito].timer_max, 0.0f, 10.0f);
        ImGui::SliderInt(u8"otˇéUÍivXĚŞj", &unit_status[UNIT_INDEX::Shishito].streng_width, 1, 5);
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("green_pumpkin"))
    {
        UnitImGui(unit_status[UNIT_INDEX::GreenPumpkin]);
        ImGui::SliderFloat(u8"Op`Ěł", &unit_status[UNIT_INDEX::GreenPumpkin].t_height, 0.0f, 5.0f);
        ImGui::SliderFloat(u8"Op`ĚęÓˇ", &unit_status[UNIT_INDEX::GreenPumpkin].t_base, 0.0f, 5.0f);
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("orange_pumpkin"))
    {
        UnitImGui(unit_status[UNIT_INDEX::OrangePumpkin]);
        ImGui::SliderFloat(u8"Op`Ěł", &unit_status[UNIT_INDEX::OrangePumpkin].t_height, 0.0f, 5.0f);
        ImGui::SliderFloat(u8"Op`ĚęÓˇ", &unit_status[UNIT_INDEX::OrangePumpkin].t_base, 0.0f, 5.0f);
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Broccoli"))
    {
        UnitImGui(unit_status[UNIT_INDEX::Broccoli]);
        ImGui::SliderFloat(u8"U", &unit_status[UNIT_INDEX::Broccoli].attack_width, 0.1f, 10.0f);
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Cauliflower"))
    {
        UnitImGui(unit_status[UNIT_INDEX::Cauliflower]);
        ImGui::SliderFloat(u8"U", &unit_status[UNIT_INDEX::Cauliflower].attack_width, 0.1f, 10.0f);
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Unit_J"))
    {
        UnitImGui(unit_status[UNIT_INDEX::J]);
        ImGui::SliderFloat(u8"h[icźa", &unit_status[UNIT_INDEX::J].radius_in, 0.0f, 5.0f);
        ImGui::SliderFloat(u8"h[icOźa", &unit_status[UNIT_INDEX::J].attack_radius, 0.0f, 5.0f);
        ImGui::TreePop();
    }
    ImGui::End();
}

void UnitManager::SetUpUnit(Unit* unit, int unit_index)
{
    unit->SetAttackTimes(unit_status[unit_index].basic.attack_times);
    unit->SetAttackPower(unit_status[unit_index].basic.attack_power);
    unit->SetAttackInterval(unit_status[unit_index].basic.attack_interval);
    unit->SetAttackEffectSize(unit_status[unit_index].basic.attack_effect_size);
    unit->SetDeathEffectSize(unit_status[unit_index].basic.death_effect_size);
    unit->SetSetEffectSize(unit_status[unit_index].basic.set_effect_size);
    unit->SetAttackInterval(unit_status[unit_index].basic.attack_interval);
    unit->SetAttackInterval(unit_status[unit_index].basic.attack_interval);
    unit->SetAttackRadius(unit_status[unit_index].attack_radius);
    unit->SetTimerMax(unit_status[unit_index].timer_max);
    unit->SetStrengWidth(unit_status[unit_index].streng_width);
    unit->SetTHeight(unit_status[unit_index].t_height);
    unit->SetTBase(unit_status[unit_index].t_base);
    unit->SetAttackWidth(unit_status[unit_index].attack_width);
    unit->SetAttackRadiusIn(unit_status[unit_index].radius_in);
}
