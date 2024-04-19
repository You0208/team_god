#pragma once
#pragma once

#include<vector>
#include<set>
#include "Unit.h"

// ���j�b�g�}�l�[�W���[
class UnitManager
{
private:
    UnitManager() {}
    ~UnitManager() {}
public:
    // �B��̃C���X�^���X�擾
    static UnitManager& Instance()
    {
        static UnitManager instance;
        return instance;
    }
    // ������
    void Initialize();

    // �X�V����
    void Update(float elapsedTime);

    // �`�揈��
    void Render(float scale, ID3D11PixelShader** replaced_pixel_shader);

    // ���j�b�g�o�^
    void Register(Unit* unit);

    // ���j�b�g�S�폜
    void Clear();

    //�f�o�b�O�v���~�e�B�u�`��
    void DrawDebugPrimitive();

    // ���j�b�g���擾
    int GetUnitCount() const { return static_cast<int>(units.size()); }

    // ���j�b�g�擾
    Unit* GetUnit(int index) { return units.at(index); }

    // ���j�b�g�폜
    void Remove(Unit* units);

    // Imgui
    void DebugImGui();
private:
    std::vector<Unit*> units;
    std::set<Unit*>    removes;


    //-----���̉��f�o�b�O�p��--------------------------------------------
    struct UnitStatus
    {
        int attack_power;//�U����
        float attack_interval;//�U���Ԋu
        int attack_times; // �U����
    };
    void UnitImGui(UnitStatus& status) {
        ImGui::SliderInt(u8"�U����", &status.attack_times, 1, 10);
        ImGui::SliderInt(u8"�U����", &status.attack_power, 0, 10);
        ImGui::SliderFloat(u8"�U���Ԋu", &status.attack_interval, 0.0f, 10.0f);
    }

    // Unit_A
    UnitStatus unit_A;
    float radius_A;  // ���a

    // Unit_B
    UnitStatus unit_B;
    float t_height_B;
    float t_base_B;

    // Unit_C
    UnitStatus unit_C;
    float t_height_C;
    float t_base_C;

    // Unit_D
    float radius_D;  // ���a
    float timer_max_D;
    int streng_width_D;

    // Unit_E
    UnitStatus unit_E;
    float attack_width_E;

    // Unit_F
    UnitStatus unit_F;
    float attack_width_F;

public:
    void SetUpUnit_A(Unit* unit);
    void SetUpUnit_B(Unit* unit);
    void SetUpUnit_C(Unit* unit);
    void SetUpUnit_D(Unit* unit);
    void SetUpUnit_E(Unit* unit);
    void SetUpUnit_F(Unit* unit);
};