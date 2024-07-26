#include "UnitManager.h"
#include "Lemur/Graphics/Graphics.h"
#include "Lemur/Collision/Collision.h"

void UnitManager::Initialize()
{
    // モデル前読み込み
    {
        Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();

        model_1 = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Unit\\Chili.fbx");
        model_2 = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Unit\\Shishito.fbx");
        model_3 = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Unit\\OrangePumpkin.fbx");
        model_4 = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Unit\\GreenPumpkin.fbx");
        model_5 = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Unit\\Broccoli.fbx");
        model_6 = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Unit\\Cauliflower.fbx");
        model_7 = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Unit\\Mustard.fbx");
    }

    //TODO もね　ユニットステータス
    {
        //Chili
        InitializeBasic(unit_status[UNIT_INDEX::Chili],
            0.0f,// 攻撃間隔
            3,   // 攻撃力
            1,   // 攻撃回数
            0.4f,// 攻撃エフェクトサイズ
            0.3f,// 死亡エフェクトサイズ
            0.3f // 設置エフェクトサイズ
        );
        unit_status[UNIT_INDEX::Chili].attack_radius                  = 1.60f;                 // 攻撃半径

        //Shishito
        InitializeBasic(unit_status[UNIT_INDEX::Shishito],
            0.0f,// 攻撃間隔
            0,   // 攻撃力
            0,   // 攻撃回数
            0.25f,// 攻撃エフェクトサイズ
            0.3f,// 死亡エフェクトサイズ
            0.3f // 設置エフェクトサイズ
        );
        unit_status[UNIT_INDEX::Shishito].attack_radius = 3.0f;                 // 攻撃半径
        unit_status[UNIT_INDEX::Shishito].timer_max = 7.0f;                 // 出現時間
        unit_status[UNIT_INDEX::Shishito].streng_width = 3;                    // 攻撃力の上り幅

        //OrangePumpkin
        InitializeBasic(unit_status[UNIT_INDEX::OrangePumpkin],
            0.0f,// 攻撃間隔
            5,   // 攻撃力R
            1,   // 攻撃回数
            0.4f,// 攻撃エフェクトサイズ
            0.3f,// 死亡エフェクトサイズ
            0.3f // 設置エフェクトサイズ
        );
        unit_status[UNIT_INDEX::OrangePumpkin].t_height = 1.7f;             // 三角形の高さ
        unit_status[UNIT_INDEX::OrangePumpkin].t_base = 1.67f;             // 三角形の底辺長

        //GreenPumpkin
        InitializeBasic(unit_status[UNIT_INDEX::GreenPumpkin],
            0.0f,// 攻撃間隔
            5,   // 攻撃力
            1,   // 攻撃回数
            0.15f,// 攻撃エフェクトサイズ
            0.3f,// 死亡エフェクトサイズ
            0.3f // 設置エフェクトサイズ
        );
        unit_status[UNIT_INDEX::GreenPumpkin].t_height = 1.7f;                // 三角形の高さ
        unit_status[UNIT_INDEX::GreenPumpkin].t_base   = 1.67f;                // 三角形の底辺長

        //Broccoli
        InitializeBasic(unit_status[UNIT_INDEX::Broccoli],
            0.0f,// 攻撃間隔
            1,   // 攻撃力
            1,   // 攻撃回数
            2.0f,// 攻撃エフェクトサイズ
            0.3f,// 死亡エフェクトサイズ
            0.3f // 設置エフェクトサイズ
        );
        unit_status[UNIT_INDEX::Broccoli].attack_width            = 0.9f;                 // 攻撃範囲の幅

        //Cauliflower
        InitializeBasic(unit_status[UNIT_INDEX::Cauliflower],
            0.0f,// 攻撃間隔
            1,   // 攻撃力
            1,   // 攻撃回数
            2.0f,// 攻撃エフェクトサイズ
            0.3f,// 死亡エフェクトサイズ
            0.3f // 設置エフェクトサイズ
        );
        unit_status[UNIT_INDEX::Cauliflower].attack_width = 0.9f;                 // 攻撃範囲の幅

        //Unit_J
        InitializeBasic(unit_status[UNIT_INDEX::J],
            0.0f,// 攻撃間隔
            3,   // 攻撃力
            1,   // 攻撃回数
            0.6f,// 攻撃エフェクトサイズ
            0.3f,// 死亡エフェクトサイズ
            0.3f // 設置エフェクトサイズ
        );               
        unit_status[UNIT_INDEX::J].radius_in =0.9f;                  // 半径
        unit_status[UNIT_INDEX::J].attack_radius =1.6f;                  // 半径
    }
}

// 更新処理
void UnitManager::Update(float elapsedTime)
{
#ifdef DEBUG_IMGUI
    DebugImGui();
#endif // DEBUG_IMGUI

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
#ifdef DEBUG_IMGUI
        unit->DrawDebugGUI(i);
#endif //DEBUG_IMGUI
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
#ifdef DEBUG_IMGUI
    DrawDebugPrimitive();
#endif // DEBUG_IMGUI

    for (Unit* unit : units)
    {
        unit->Render(elapsedTime, replaced_pixel_shader);
    }
}

void UnitManager::CollisionRender(float scale, ID3D11PixelShader* replaced_pixel_shader, ID3D11PixelShader* replaced_pixel_shader2)
{
    DrawDebugPrimitive();
    for (Unit* unit : units)
    {
        if (unit->GetCategory() == UNIT_INDEX::Shishito)
        {
            unit->CollisionRender(scale, replaced_pixel_shader2);
        }
        else if (unit->GetCategory() == UNIT_INDEX::GreenPumpkin)
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
    if (ImGui::TreeNode("Unit_J"))
    {
        UnitImGui(unit_status[UNIT_INDEX::J]);
        ImGui::SliderFloat(u8"ドーナツ中半径", &unit_status[UNIT_INDEX::J].radius_in, 0.0f, 5.0f);
        ImGui::SliderFloat(u8"ドーナツ外半径", &unit_status[UNIT_INDEX::J].attack_radius, 0.0f, 5.0f);
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
