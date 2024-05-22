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
    void Render(float scale, ID3D11PixelShader* replaced_pixel_shader);
    void CollisionRender(float scale, ID3D11PixelShader* replaced_pixel_shader, ID3D11PixelShader* replaced_pixel_shader2);

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

public:

    enum UNIT_INDEX
    {
        Chili,
        Shishito,
        OrangePumpkin,
        GreenPumpkin,
        Broccoli,
        Cauliflower,
        J
    };

private:
    std::vector<Unit*> units;
    std::set<Unit*>    removes;

    std::shared_ptr<FbxModelManager> model_1 = nullptr;
    std::shared_ptr<FbxModelManager> model_2 = nullptr;
    std::shared_ptr<FbxModelManager> model_3 = nullptr;
    std::shared_ptr<FbxModelManager> model_4 = nullptr;
    std::shared_ptr<FbxModelManager> model_5 = nullptr;
    std::shared_ptr<FbxModelManager> model_6 = nullptr;
    std::shared_ptr<FbxModelManager> model_7 = nullptr;

    //-----この下デバッグ用↓--------------------------------------------
    struct UnitStatus
    {
        // 基本ステータス
        struct Basic
        {
            float               attack_interval;    //攻撃間隔
            int                 attack_power;       //攻撃力
            int                 attack_times;       // 攻撃回数
            float               attack_effect_size; // 攻撃エフェクトのサイズ
            float               death_effect_size;  // 死亡エフェクトのサイズ
            float               set_effect_size;    // 設置エフェクトのサイズ
        };
        Basic basic;

        // 各ステータス
        float               attack_radius;      // 攻撃半径
        float               timer_max;          // 存在時間
        int                 streng_width;       // バフ量
        float               t_height;           // 三角形高さ
        float               t_base;             // 三角形底辺長
        float               attack_width;       //　攻撃幅
        float               radius_in;         // ドーナツ内側半径
    };

    void UnitImGui(UnitStatus& status) {
        ImGui::SliderInt(u8"攻撃回数", &status.basic.attack_times, 1, 10);
        ImGui::SliderInt(u8"攻撃力", &status.basic.attack_power, 0, 10);
        ImGui::SliderFloat(u8"攻撃間隔", &status.basic.attack_interval, 0.0f, 10.0f);
        ImGui::SliderFloat(u8"攻撃エフェクトのサイズ", &status.basic.attack_effect_size, 0.0f, 2.0f);
        ImGui::SliderFloat(u8"死亡エフェクトのサイズ", &status.basic.death_effect_size, 0.0f, 2.0f);
        ImGui::SliderFloat(u8"設置エフェクトのサイズ", &status.basic.set_effect_size, 0.0f, 2.0f);
    }

    void InitializeBasic(
        UnitStatus& unit,
        float attack_interval,    //攻撃間隔
        int attack_power,       //攻撃力
        int attack_times,       // 攻撃回数
        float attack_effect_size, // 攻撃エフェクトのサイズ
        float death_effect_size,  // 死亡エフェクトのサイズ
        float set_effect_size    // 設置エフェクトのサイズ
        )
    {
        unit = {};
        unit.basic.attack_interval = attack_interval;
        unit.basic.attack_power = attack_power;
        unit.basic.attack_times = attack_times;
        unit.basic.attack_effect_size = attack_effect_size;
        unit.basic.death_effect_size = death_effect_size;
        unit.basic.set_effect_size = set_effect_size;
    }

    UnitStatus unit_status[7];
  
public:
    void SetUpUnit(Unit* unit, int unit_index);
};