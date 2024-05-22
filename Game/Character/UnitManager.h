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
    void CollisionRender(float scale, ID3D11PixelShader* replaced_pixel_shader, ID3D11PixelShader* replaced_pixel_shader2);

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

    UnitStatus unit_status[7];
  
public:
    void SetUpUnit(Unit* unit, int unit_index);
};