#pragma once
#pragma once

#include<vector>
#include<set>
#include "Unit.h"

// ユニットマネージャー
class UnitManager
{
private:
    UnitManager() {}
    ~UnitManager() {}
public:
    // 唯一のインスタンス取得
    static UnitManager& Instance()
    {
        static UnitManager instance;
        return instance;
    }
    // 初期化
    void Initialize();

    // 更新処理
    void Update(float elapsedTime);

    // 描画処理
    void Render(float scale, ID3D11PixelShader** replaced_pixel_shader);

    // ユニット登録
    void Register(Unit* unit);

    // ユニット全削除
    void Clear();

    //デバッグプリミティブ描画
    void DrawDebugPrimitive();

    // ユニット数取得
    int GetUnitCount() const { return static_cast<int>(units.size()); }

    // ユニット取得
    Unit* GetUnit(int index) { return units.at(index); }

    // ユニット削除
    void Remove(Unit* units);

    // Imgui
    void DebugImGui();
private:
    std::vector<Unit*> units;
    std::set<Unit*>    removes;


    //-----この下デバッグ用↓--------------------------------------------
    struct UnitStatus
    {
        int attack_power;//攻撃力
        float attack_interval;//攻撃間隔
        int attack_times; // 攻撃回数
    };
    void UnitImGui(UnitStatus& status) {
        ImGui::SliderInt(u8"攻撃回数", &status.attack_times, 1, 10);
        ImGui::SliderInt(u8"攻撃力", &status.attack_power, 0, 10);
        ImGui::SliderFloat(u8"攻撃間隔", &status.attack_interval, 0.0f, 10.0f);
    }

    // Unit_A
    UnitStatus unit_A;
    float radius_A;  // 半径

    // Unit_B
    UnitStatus unit_B;
    float t_height_B;
    float t_base_B;

    // Unit_C
    UnitStatus unit_C;
    float t_height_C;
    float t_base_C;

    // Unit_D
    float radius_D;  // 半径
    float timer_max_D;
    int streng_width_D;

    // Unit_E
    UnitStatus unit_E;
    float attack_width_E;

    // Unit_F
    UnitStatus unit_F;
    float attack_width_F;

    // Unit_H
    UnitStatus unit_H;
    DirectX::XMFLOAT2 attack_width_H;
    float rect_angle_H;

    // Unit_I
    UnitStatus unit_I;
    DirectX::XMFLOAT2 attack_width_I;
    float rect_angle_I;

    // Unit_J
    UnitStatus unit_J;
    float radius_in_J;  // 半径
    float radius_out_J;  // 半径
public:
    void SetUpUnit_A(Unit* unit);
    void SetUpUnit_B(Unit* unit);
    void SetUpUnit_C(Unit* unit);
    void SetUpUnit_D(Unit* unit);
    void SetUpUnit_E(Unit* unit);
    void SetUpUnit_F(Unit* unit);
    void SetUpUnit_H(Unit* unit);
    void SetUpUnit_I(Unit* unit);
    void SetUpUnit_J(Unit* unit);
};