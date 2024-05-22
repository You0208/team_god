#include "UnitManager.h"
#include "Lemur/Graphics/Graphics.h"
#include "Lemur/Collision/Collision.h"

void UnitManager::Initialize()
{
    // fOÇÝÝ
    {
        Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();

        model_1 = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Unit\\Chili.fbx");
        model_2 = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Unit\\Shishito.fbx");
        model_3 = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Unit\\OrangePumpkin.fbx");
        model_4 = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Unit\\GreenPumpkin.fbx");
        model_5 = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Unit\\Broccoli.fbx");
        model_6 = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Unit\\Cauliflower.fbx");
    }

    //TODO àË@jbgXe[^X
    {
        //Chili
        InitializeBasic(unit_status[UNIT_INDEX::Chili],
            0.0f,// UÔu
            3,   // UÍ
            1,   // Uñ
            0.3f,// UGtFNgTCY
            0.3f,// SGtFNgTCY
            0.3f // ÝuGtFNgTCY
        );
        unit_status[UNIT_INDEX::Chili].attack_radius                  = 1.60f;                 // U¼a

        //Shishito
        InitializeBasic(unit_status[UNIT_INDEX::Shishito],
            0.0f,// UÔu
            0,   // UÍ
            0,   // Uñ
            0.25f,// UGtFNgTCY
            0.3f,// SGtFNgTCY
            0.3f // ÝuGtFNgTCY
        );
        unit_status[UNIT_INDEX::Shishito].attack_radius = 3.0f;                 // U¼a
        unit_status[UNIT_INDEX::Shishito].timer_max = 7.0f;                 // o»Ô
        unit_status[UNIT_INDEX::Shishito].streng_width = 3;                    // UÍÌãè

        //OrangePumpkin
        InitializeBasic(unit_status[UNIT_INDEX::OrangePumpkin],
            0.0f,// UÔu
            5,   // UÍR
            1,   // Uñ
            0.3f,// UGtFNgTCY
            0.3f,// SGtFNgTCY
            0.3f // ÝuGtFNgTCY
        );
        unit_status[UNIT_INDEX::OrangePumpkin].t_height = 1.7f;             // Op`Ì³
        unit_status[UNIT_INDEX::OrangePumpkin].t_base = 1.67f;             // Op`ÌêÓ·

        //GreenPumpkin
        InitializeBasic(unit_status[UNIT_INDEX::GreenPumpkin],
            0.0f,// UÔu
            5,   // UÍ
            1,   // Uñ
            0.1f,// UGtFNgTCY
            0.3f,// SGtFNgTCY
            0.3f // ÝuGtFNgTCY
        );
        unit_status[UNIT_INDEX::GreenPumpkin].t_height = 1.7f;                // Op`Ì³
        unit_status[UNIT_INDEX::GreenPumpkin].t_base   = 1.67f;                // Op`ÌêÓ·

        //Broccoli
        InitializeBasic(unit_status[UNIT_INDEX::Broccoli],
            0.0f,// UÔu
            1,   // UÍ
            1,   // Uñ
            1.0f,// UGtFNgTCY
            0.3f,// SGtFNgTCY
            0.3f // ÝuGtFNgTCY
        );
        unit_status[UNIT_INDEX::Broccoli].attack_width            = 0.9f;                 // UÍÍÌ

        //Cauliflower
        InitializeBasic(unit_status[UNIT_INDEX::Cauliflower],
            0.0f,// UÔu
            1,   // UÍ
            1,   // Uñ
            0.5f,// UGtFNgTCY
            0.3f,// SGtFNgTCY
            0.3f // ÝuGtFNgTCY
        );
        unit_status[UNIT_INDEX::Cauliflower].attack_width = 0.9f;                 // UÍÍÌ

        //Unit_J
        InitializeBasic(unit_status[UNIT_INDEX::J],
            0.0f,// UÔu
            3,   // UÍ
            1,   // Uñ
            0.5f,// UGtFNgTCY
            0.3f,// SGtFNgTCY
            0.3f // ÝuGtFNgTCY
        );               
        unit_status[UNIT_INDEX::J].radius_in =0.9f;                  // ¼a
        unit_status[UNIT_INDEX::J].attack_radius =1.6f;                  // ¼a
    }
}

// XV
void UnitManager::Update(float elapsedTime)
{
    //TODO ImGuiÁ·
    //DebugImGui();
    for (Unit* unit : units)
    {
        unit->Update(elapsedTime);
        if (unit->GetCategory() == UNIT_INDEX::Broccoli)
        {
            unit->collision_pos = {
                0,
                unit->GetPosition().y,
                unit->GetPosition().z
            };
            unit->collision_scale = {
                unit->GetScale().x * 6,
                unit->GetScale().y,
               0.6f
            };
        }
        else if (unit->GetCategory() == UNIT_INDEX::Cauliflower)
        {
            unit->collision_pos = {
                unit->GetPosition().x,
                unit->GetPosition().y,
                0
            };
            unit->collision_scale = {
               0.6f,
                unit->GetScale().y,
                unit->GetScale().z * 6
            };
        }
        else if (unit->GetCategory() == UNIT_INDEX::OrangePumpkin)
        {
            unit->collision_pos = unit->GetPosition();
            unit->collision_scale = {
                unit->GetScale().x,
                unit->GetScale().y,
                unit->GetScale().z
            };
            unit->collision_rotation = {
                0.0f,
                DirectX::XMConvertToRadians(90.0f),
                0.0f
            };
        }
        else
        {
            unit->collision_pos = unit->GetPosition();
            unit->collision_scale = unit->GetScale();
        }
    }

    for (int i = 0; i < GetUnitCount(); i++)
    {
        Unit* unit = units.at(i);
        //TODO ImGuiÁ·
        //unit->DrawDebugGUI(i);
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
    // jüXgðNA
    removes.clear();

}

// `æ
void UnitManager::Render(float elapsedTime, ID3D11PixelShader* replaced_pixel_shader)
{
    DrawDebugPrimitive();
    for (Unit* unit : units)
    {
        unit->Render(elapsedTime, replaced_pixel_shader);
    }
}

void UnitManager::CollisionRender(float scale, ID3D11PixelShader* replaced_pixel_shader)
{
    DrawDebugPrimitive();
    for (Unit* unit : units)
    {
        if (unit->GetCategory() == UNIT_INDEX::GreenPumpkin)
        {
            unit->CollisionRender(scale * 70, replaced_pixel_shader);
        }
        else if (unit->GetCategory() == UNIT_INDEX::OrangePumpkin)
        {
            unit->CollisionRender(scale * 70, replaced_pixel_shader);
        }
        else if (unit->GetCategory() == UNIT_INDEX::J)
        {
            unit->CollisionRender(scale * 70, replaced_pixel_shader);
        }
        else
        {
            unit->CollisionRender(scale, replaced_pixel_shader);
        }
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
        ImGui::SliderFloat(u8"U¼a", &unit_status[UNIT_INDEX::Chili].attack_radius, 0.0f, 5.0f);
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("shishito"))
    {
        ImGui::SliderFloat(u8"U¼a", &unit_status[UNIT_INDEX::Shishito].attack_radius, 0.0f, 5.0f);
        ImGui::SliderFloat(u8"o»Ô", &unit_status[UNIT_INDEX::Shishito].timer_max, 0.0f, 10.0f);
        ImGui::SliderInt(u8"ot·éUÍivXÌªj", &unit_status[UNIT_INDEX::Shishito].streng_width, 1, 5);
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("green_pumpkin"))
    {
        UnitImGui(unit_status[UNIT_INDEX::GreenPumpkin]);
        ImGui::SliderFloat(u8"Op`Ì³", &unit_status[UNIT_INDEX::GreenPumpkin].t_height, 0.0f, 5.0f);
        ImGui::SliderFloat(u8"Op`ÌêÓ·", &unit_status[UNIT_INDEX::GreenPumpkin].t_base, 0.0f, 5.0f);
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("orange_pumpkin"))
    {
        UnitImGui(unit_status[UNIT_INDEX::OrangePumpkin]);
        ImGui::SliderFloat(u8"Op`Ì³", &unit_status[UNIT_INDEX::OrangePumpkin].t_height, 0.0f, 5.0f);
        ImGui::SliderFloat(u8"Op`ÌêÓ·", &unit_status[UNIT_INDEX::OrangePumpkin].t_base, 0.0f, 5.0f);
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
        ImGui::SliderFloat(u8"h[ic¼a", &unit_status[UNIT_INDEX::J].radius_in, 0.0f, 5.0f);
        ImGui::SliderFloat(u8"h[icO¼a", &unit_status[UNIT_INDEX::J].attack_radius, 0.0f, 5.0f);
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
