#include "UnitManager.h"
#include "Lemur/Graphics/Graphics.h"
#include "Lemur/Collision/Collision.h"

void UnitManager::Initialize()
{
    //TODO もね　ユニットステータス
    {
        //Chili
        InitializeBasic(unit_status[UNIT_INDEX::Chili],
            0.0f,// 攻撃間隔
            3,   // 攻撃力
            1,   // 攻撃回数
            0.5f,// 攻撃エフェクトサイズ
            0.5f,// 死亡エフェクトサイズ
            0.5f // 設置エフェクトサイズ
        );
        unit_status[UNIT_INDEX::Chili].attack_radius                  = 1.5f;                 // 攻撃半径

        //OrangePumpkin
        InitializeBasic(unit_status[UNIT_INDEX::OrangePumpkin],
            0.1f,// 攻撃間隔
            4,   // 攻撃力
            1,   // 攻撃回数
            0.5f,// 攻撃エフェクトサイズ
            0.5f,// 死亡エフェクトサイズ
            0.5f // 設置エフェクトサイズ
        );
        unit_status[UNIT_INDEX::OrangePumpkin].t_height = 1.59f;             // 三角形の高さ
        unit_status[UNIT_INDEX::OrangePumpkin].t_base = 1.59f;             // 三角形の底辺長

        //GreenPumpkin
        InitializeBasic(unit_status[UNIT_INDEX::GreenPumpkin],
            0.1f,// 攻撃間隔
            4,   // 攻撃力
            1,   // 攻撃回数
            0.5f,// 攻撃エフェクトサイズ
            0.5f,// 死亡エフェクトサイズ
            0.5f // 設置エフェクトサイズ
        );
        unit_status[UNIT_INDEX::GreenPumpkin].t_height = 1.59f;                // 三角形の高さ
        unit_status[UNIT_INDEX::GreenPumpkin].t_base   = 1.59f;                // 三角形の底辺長

        //Shishito
         InitializeBasic(unit_status[UNIT_INDEX::Shishito],
             0.0f,// 攻撃間隔
             0,   // 攻撃力
             0,   // 攻撃回数
             0.5f,// 攻撃エフェクトサイズ
             0.5f,// 死亡エフェクトサイズ
             0.5f // 設置エフェクトサイズ
         );
         unit_status[UNIT_INDEX::Shishito].attack_radius   = 3.0f;                 // 攻撃半径
         unit_status[UNIT_INDEX::Shishito].timer_max              = 5.0f;                 // 出現時間
         unit_status[UNIT_INDEX::Shishito].streng_width           = 2;                    // 攻撃力の上り幅

        //Broccoli
        InitializeBasic(unit_status[UNIT_INDEX::Broccoli],
            0.1f,// 攻撃間隔
            1,   // 攻撃力
            1,   // 攻撃回数
            0.5f,// 攻撃エフェクトサイズ
            0.5f,// 死亡エフェクトサイズ
            0.5f // 設置エフェクトサイズ
        );
        unit_status[UNIT_INDEX::Broccoli].attack_width            = 0.9f;                 // 攻撃範囲の幅

        //Cauliflower
        InitializeBasic(unit_status[UNIT_INDEX::Cauliflower],
            0.1f,// 攻撃間隔
            1,   // 攻撃力
            1,   // 攻撃回数
            0.5f,// 攻撃エフェクトサイズ
            0.5f,// 死亡エフェクトサイズ
            0.5f // 設置エフェクトサイズ
        );
        unit_status[UNIT_INDEX::Cauliflower].attack_width = 0.9f;                 // 攻撃範囲の幅

        //Unit_H
        InitializeBasic(unit_status[UNIT_INDEX::H],
            0.1f,// 攻撃間隔
            1,   // 攻撃力
            1,   // 攻撃回数
            0.5f,// 攻撃エフェクトサイズ
            0.5f,// 死亡エフェクトサイズ
            0.5f // 設置エフェクトサイズ
        );             
        unit_status[UNIT_INDEX::H].attack_width_2            = {1.0f,20.0f};         // 攻撃範囲の幅
        unit_status[UNIT_INDEX::H].rect_angle              = 50.0f;                // 攻撃範囲の幅

        //Unit_I
        InitializeBasic(unit_status[UNIT_INDEX::I],
            0.1f,// 攻撃間隔
            1,   // 攻撃力
            1,   // 攻撃回数
            0.5f,// 攻撃エフェクトサイズ
            0.5f,// 死亡エフェクトサイズ
            0.5f // 設置エフェクトサイズ
        );
        unit_status[UNIT_INDEX::I].attack_width_2 = { 1.0f,20.0f };         // 攻撃範囲の幅
        unit_status[UNIT_INDEX::I].rect_angle = 50.0f;                // 攻撃範囲の幅

        //Unit_J
        InitializeBasic(unit_status[UNIT_INDEX::J],
            0.1f,// 攻撃間隔
            1,   // 攻撃力
            1,   // 攻撃回数
            0.5f,// 攻撃エフェクトサイズ
            0.5f,// 死亡エフェクトサイズ
            0.5f // 設置エフェクトサイズ
        );               
        unit_status[UNIT_INDEX::J].radius_in =1.0f;                  // 半径
        unit_status[UNIT_INDEX::J].attack_radius =2.0f;                  // 半径

    }
}

// 更新処理
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
    // 破棄リストに追加
    removes.insert(unit);
}

void UnitManager::DebugImGui()
{
    ImGui::Begin(u8"ユニット");
    if (ImGui::TreeNode("chili"))
    {
        UnitImGui(unit_status[UNIT_INDEX::Chili]);
        ImGui::SliderFloat(u8"攻撃半径", &unit_status[UNIT_INDEX::Chili].attack_radius, 0.0f, 5.0f);
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("shishito"))
    {
        ImGui::SliderFloat(u8"攻撃半径", &unit_status[UNIT_INDEX::Shishito].attack_radius, 0.0f, 5.0f);
        ImGui::SliderFloat(u8"出現時間", &unit_status[UNIT_INDEX::Shishito].timer_max, 0.0f, 10.0f);
        ImGui::SliderInt(u8"バフする攻撃力（プラスの分）", &unit_status[UNIT_INDEX::Shishito].streng_width, 1, 5);
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("green_pumpkin"))
    {
        UnitImGui(unit_status[UNIT_INDEX::GreenPumpkin]);
        ImGui::SliderFloat(u8"三角形の高さ", &unit_status[UNIT_INDEX::GreenPumpkin].t_height, 0.0f, 5.0f);
        ImGui::SliderFloat(u8"三角形の底辺長", &unit_status[UNIT_INDEX::GreenPumpkin].t_base, 0.0f, 5.0f);
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("orange_pumpkin"))
    {
        UnitImGui(unit_status[UNIT_INDEX::OrangePumpkin]);
        ImGui::SliderFloat(u8"三角形の高さ", &unit_status[UNIT_INDEX::OrangePumpkin].t_height, 0.0f, 5.0f);
        ImGui::SliderFloat(u8"三角形の底辺長", &unit_status[UNIT_INDEX::OrangePumpkin].t_base, 0.0f, 5.0f);
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Broccoli"))
    {
        UnitImGui(unit_status[UNIT_INDEX::Broccoli]);
        ImGui::SliderFloat(u8"攻撃幅", &unit_status[UNIT_INDEX::Broccoli].attack_width, 0.1f, 10.0f);
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Cauliflower"))
    {
        UnitImGui(unit_status[UNIT_INDEX::Cauliflower]);
        ImGui::SliderFloat(u8"攻撃幅", &unit_status[UNIT_INDEX::Cauliflower].attack_width, 0.1f, 10.0f);
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Unit_H"))
    {
        UnitImGui(unit_status[UNIT_INDEX::H]);
        ImGui::SliderFloat2(u8"攻撃幅", &unit_status[UNIT_INDEX::H].attack_width_2.x, 0.1f, 20.0f);
        ImGui::SliderFloat(u8"攻撃角度", &unit_status[UNIT_INDEX::H].rect_angle, 0.0f, 360.0f);
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Unit_I"))
    {
        UnitImGui(unit_status[UNIT_INDEX::I]);
        ImGui::SliderFloat2(u8"攻撃幅", &unit_status[UNIT_INDEX::I].attack_width_2.x, 0.1f, 20.0f);
        ImGui::SliderFloat(u8"攻撃角度", &unit_status[UNIT_INDEX::I].rect_angle, 0.0f, 360.0f);
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Unit_J"))
    {
        UnitImGui(unit_status[UNIT_INDEX::J]);
        ImGui::SliderFloat(u8"ドーナツ中半径", &unit_status[UNIT_INDEX::I].radius_in, 0.0f, 5.0f);
        ImGui::SliderFloat(u8"ドーナツ外半径", &unit_status[UNIT_INDEX::I].attack_radius, 0.0f, 5.0f);
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
    unit->SetAttackWidth(unit_status[unit_index].attack_width_2);
    unit->SetRectAngle(unit_status[unit_index].rect_angle);
    unit->SetAttackRadiusIn(unit_status[unit_index].radius_in);
}

//
//void UnitManager::SetUpUnit_A(Unit* unit)
//{
//    unit->SetAttackTimes(chili.attack_times);
//    unit->SetAttackPower(chili.attack_power);
//    unit->SetAttackInterval(chili.attack_interval);
//    unit->SetAttackEffectSize(chili.attack_effect_size);
//    unit->SetDeathEffectSize(chili.death_effect_size);
//    unit->SetSetEffectSize(chili.set_effect_size);
//    unit->SetAttackInterval(chili.attack_interval);
//    unit->SetAttackInterval(chili.attack_interval);
//    unit->SetAttackRadius(radius_chili);
//    unit->SetTimerMax(timer_max_shishito);
//    unit->SetStrengWidth(streng_width_shishito);
//}
//
//void UnitManager::SetUpUnit_B(Unit* unit)
//{
//    unit->SetAttackTimes(green_pumpkin.attack_times);
//    unit->SetAttackPower(green_pumpkin.attack_power);
//    unit->SetAttackInterval(green_pumpkin.attack_interval);
//    unit->SetAttackEffectSize(green_pumpkin.attack_effect_size);
//    unit->SetDeathEffectSize(green_pumpkin.death_effect_size);
//    unit->SetSetEffectSize(green_pumpkin.set_effect_size);
//    unit->SetTHeight(t_height_B);
//    unit->SetTBase(t_base_B);
//}
//
//void UnitManager::SetUpUnit_C(Unit* unit)
//{
//    unit->SetAttackTimes(orange_pumpkin.attack_times);
//    unit->SetAttackPower(orange_pumpkin.attack_power);
//    unit->SetAttackInterval(orange_pumpkin.attack_interval);
//    unit->SetAttackEffectSize(orange_pumpkin.attack_effect_size);
//    unit->SetDeathEffectSize(orange_pumpkin.death_effect_size);
//    unit->SetSetEffectSize(orange_pumpkin.set_effect_size);
//    unit->SetTHeight(t_height_C);
//    unit->SetTBase(t_base_C);
//}
//
//void UnitManager::SetUpUnit_D(Unit* unit)
//{
//    unit->SetAttackRadius(radius_shishito);
//    unit->SetTimerMax(timer_max_shishito);
//    unit->SetStrengWidth(streng_width_shishito);
//    unit->SetAttackEffectSize(attack_effect_size_shishito);
//    unit->SetDeathEffectSize(death_effect_size_shishito);
//    unit->SetSetEffectSize(set_effect_size_shishito);
//}
//
//void UnitManager::SetUpUnit_E(Unit* unit)
//{
//    unit->SetAttackTimes(unit_E.attack_times);
//    unit->SetAttackPower(unit_E.attack_power);
//    unit->SetAttackInterval(unit_E.attack_interval);
//    unit->SetAttackEffectSize(unit_E.attack_effect_size);
//    unit->SetDeathEffectSize(unit_E.death_effect_size);
//    unit->SetSetEffectSize(unit_E.set_effect_size);
//    unit->SetAttackWidth(attack_width_E);
//}
//
//void UnitManager::SetUpUnit_F(Unit* unit)
//{
//    unit->SetAttackTimes(unit_F.attack_times);
//    unit->SetAttackPower(unit_F.attack_power);
//    unit->SetAttackInterval(unit_F.attack_interval);
//    unit->SetAttackEffectSize(unit_F.attack_effect_size);
//    unit->SetDeathEffectSize(unit_F.death_effect_size);
//    unit->SetSetEffectSize(unit_F.set_effect_size);
//    unit->SetAttackWidth(attack_width_F);
//}
//
//void UnitManager::SetUpUnit_H(Unit* unit)
//{
//    unit->SetAttackTimes(unit_H.attack_times);
//    unit->SetAttackPower(unit_H.attack_power);
//    unit->SetAttackInterval(unit_H.attack_interval);
//    unit->SetAttackEffectSize(unit_H.attack_effect_size);
//    unit->SetDeathEffectSize(unit_H.death_effect_size);
//    unit->SetSetEffectSize(unit_H.set_effect_size);
//    unit->SetAttackWidth(attack_width_H);
//    unit->SetRectAngle(rect_angle_H);
//}
//
//void UnitManager::SetUpUnit_I(Unit* unit)
//{
//    unit->SetAttackTimes(unit_I.attack_times);
//    unit->SetAttackPower(unit_I.attack_power);
//    unit->SetAttackInterval(unit_I.attack_interval);
//    unit->SetAttackEffectSize(unit_I.attack_effect_size);
//    unit->SetDeathEffectSize(unit_I.death_effect_size);
//    unit->SetSetEffectSize(unit_I.set_effect_size);
//    unit->SetAttackWidth(attack_width_I);
//    unit->SetRectAngle(rect_angle_I);
//}
//
//void UnitManager::SetUpUnit_J(Unit* unit)
//{
//    unit->SetAttackTimes(unit_J.attack_times);
//    unit->SetAttackPower(unit_J.attack_power);
//    unit->SetAttackInterval(unit_J.attack_interval);
//    unit->SetAttackEffectSize(unit_J.attack_effect_size);
//    unit->SetDeathEffectSize(unit_J.death_effect_size);
//    unit->SetSetEffectSize(unit_J.set_effect_size);
//    unit->SetAttackRadius(radius_in_J);
//    unit->SetAttackRadius(radius_out_J);
//}
