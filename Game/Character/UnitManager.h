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
    void Render(float scale, ID3D11PixelShader* replaced_pixel_shader);

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

public:

    enum UNIT_INDEX
    {
        Chili,
        Shishito,
        OrangePumpkin,
        GreenPumpkin,
        Broccoli,
        Cauliflower,
        H,
        I,
        J
    };

private:
    std::vector<Unit*> units;
    std::set<Unit*>    removes;


    //-----���̉��f�o�b�O�p��--------------------------------------------
    struct UnitStatus
    {
        // ��{�X�e�[�^�X
        struct Basic
        {
            float               attack_interval;    //�U���Ԋu
            int                 attack_power;       //�U����
            int                 attack_times;       // �U����
            float               attack_effect_size; // �U���G�t�F�N�g�̃T�C�Y
            float               death_effect_size;  // ���S�G�t�F�N�g�̃T�C�Y
            float               set_effect_size;    // �ݒu�G�t�F�N�g�̃T�C�Y
        };
        Basic basic;

        // �e�X�e�[�^�X
        float               attack_radius;      // �U�����a
        float               timer_max;          // ���ݎ���
        int                 streng_width;       // �o�t��
        float               t_height;           // �O�p�`����
        float               t_base;             // �O�p�`��Ӓ�
        float               attack_width;       //�@�U����
        DirectX::XMFLOAT2   attack_width_2;     //�@�U�����i�΂ߗp�j
        float               rect_angle;         // �����`�p�x
        float               radius_in;         // �h�[�i�c�������a
    };

    void UnitImGui(UnitStatus& status) {
        ImGui::SliderInt(u8"�U����", &status.basic.attack_times, 1, 10);
        ImGui::SliderInt(u8"�U����", &status.basic.attack_power, 0, 10);
        ImGui::SliderFloat(u8"�U���Ԋu", &status.basic.attack_interval, 0.0f, 10.0f);
        ImGui::SliderFloat(u8"�U���G�t�F�N�g�̃T�C�Y", &status.basic.attack_effect_size, 0.0f, 2.0f);
        ImGui::SliderFloat(u8"���S�G�t�F�N�g�̃T�C�Y", &status.basic.death_effect_size, 0.0f, 2.0f);
        ImGui::SliderFloat(u8"�ݒu�G�t�F�N�g�̃T�C�Y", &status.basic.set_effect_size, 0.0f, 2.0f);
    }

    void InitializeBasic(
        UnitStatus& unit,
        float attack_interval,    //�U���Ԋu
        int attack_power,       //�U����
        int attack_times,       // �U����
        float attack_effect_size, // �U���G�t�F�N�g�̃T�C�Y
        float death_effect_size,  // ���S�G�t�F�N�g�̃T�C�Y
        float set_effect_size    // �ݒu�G�t�F�N�g�̃T�C�Y
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

    UnitStatus unit_status[9];
  
    //// Chili
    //UnitStatus  chili;
    //float       radius_chili;  // ���a

    //// Shishito
    //float       radius_shishito;  // ���a
    //float       timer_max_shishito;
    //int         streng_width_shishito;
    //float       attack_effect_size_shishito; // �U���G�t�F�N�g�̃T�C�Y
    //float       death_effect_size_shishito;  // ���S�G�t�F�N�g�̃T�C�Y
    //float       set_effect_size_shishito;    // �ݒu�G�t�F�N�g�̃T�C�Y

    //// OrangePumpkin
    //UnitStatus  orange_pumpkin;
    //float       t_height_C;
    //float       t_base_C;

    //// GreenPumpkin
    //UnitStatus green_pumpkin;
    //float       t_height_B;
    //float       t_base_B;

    //// Broccoli
    //UnitStatus unit_E;
    //float attack_width_E;

    //// Cauliflower
    //UnitStatus unit_F;
    //float attack_width_F;

    //// Unit_H
    //UnitStatus unit_H;
    //DirectX::XMFLOAT2 attack_width_H;
    //float rect_angle_H;

    //// Unit_I
    //UnitStatus unit_I;
    //DirectX::XMFLOAT2 attack_width_I;
    //float rect_angle_I;

    //// Unit_J
    //UnitStatus unit_J;
    //float radius_in_J;  // ���a
    //float radius_out_J;  // ���a
public:
    void SetUpUnit(Unit* unit, int unit_index);

    //void SetUpUnit_A(Unit* unit);
    //void SetUpUnit_B(Unit* unit);
    //void SetUpUnit_C(Unit* unit);
    //void SetUpUnit_D(Unit* unit);
    //void SetUpUnit_E(Unit* unit);
    //void SetUpUnit_F(Unit* unit);
    //void SetUpUnit_H(Unit* unit);
    //void SetUpUnit_I(Unit* unit);
    //void SetUpUnit_J(Unit* unit);
};