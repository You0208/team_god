#include "UnitManager.h"
#include "Lemur/Graphics/Graphics.h"
#include "Lemur/Collision/Collision.h"

void UnitManager::Initialize()
{
    //TODO ���ˁ@���j�b�g�X�e�[�^�X
    {
        //Unit_A
        unit_A.attack_interval = 1.0f;  // �U���Ԋu
        unit_A.attack_power = 1;        // �U����
        unit_A.attack_times = 1;        // �U����
        radius_A = 1.0f;                // �U�����a

        //Unit_B
        unit_B.attack_interval = 1.0f;  // �U���Ԋu
        unit_B.attack_power = 1;        // �U����
        unit_B.attack_times = 1;        // �U����
        t_height_B = 1.0f;              // �O�p�`�̍���
        t_base_B = 1.0f;                // �O�p�`�̒�Ӓ�

        //Unit_C
        unit_C.attack_interval = 1.0f;  // �U���Ԋu
        unit_C.attack_power = 1;        // �U����
        unit_C.attack_times = 1;        // �U����
        t_height_C = 1.0f;              // �O�p�`�̍���
        t_base_C = 1.0f;                // �O�p�`�̒�Ӓ�

        //Unit_D
        radius_D = 1.0f;                // �U�����a
        timer_max_D = 5.0f;             // �o������
        streng_width_D = 1;             // �U���͂̏�蕝

        //Unit_E
        unit_E.attack_interval = 1.0f;  // �U���Ԋu
        unit_E.attack_power = 1;        // �U����
        unit_E.attack_times = 1;        // �U����
        attack_width_E = 1.0f;          // �U���͈͂̕�
        //Unit_F
        unit_F.attack_interval = 1.0f;  // �U���Ԋu
        unit_F.attack_power = 1;        // �U����
        unit_F.attack_times = 1;        // �U����
        attack_width_F = 1.0f;          // �U���͈͂̕�
    }
}

// �X�V����
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

    // �j������
    for (Unit* unit : removes)
    {
        std::vector<Unit*>::iterator it = std::find(units.begin(), units.end(), unit);
        if (it != units.end())
        {
            units.erase(it);
        }

        // �폜
        delete unit;
    }
    // �j�����X�g���N���A
    removes.clear();

}

// �`�揈��
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
    // �j�����X�g�ɒǉ�
    removes.insert(unit);
}

void UnitManager::DebugImGui()
{
    ImGui::Begin(u8"���j�b�g");
    if (ImGui::TreeNode("unit1"))
    {
        UnitImGui(unit_A);
        ImGui::SliderFloat(u8"�U�����a", &radius_A, 0.0f, 5.0f);
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("unit2"))
    {
        UnitImGui(unit_B);
        ImGui::SliderFloat(u8"�O�p�`�̍���", &t_height_B, 0.0f, 5.0f);
        ImGui::SliderFloat(u8"�O�p�`�̒�Ӓ�", &t_base_B, 0.0f, 5.0f);
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("unit3"))
    {
        UnitImGui(unit_C);
        ImGui::SliderFloat(u8"�O�p�`�̍���", &t_height_C, 0.0f, 5.0f);
        ImGui::SliderFloat(u8"�O�p�`�̒�Ӓ�", &t_base_C, 0.0f, 5.0f);
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("unit4"))
    {
        ImGui::SliderFloat(u8"�U�����a", &radius_D, 0.0f, 5.0f);
        ImGui::SliderFloat(u8"�o������", &timer_max_D, 0.0f, 10.0f);
        ImGui::SliderInt(u8"�o�t����U���́i�v���X�̕��j", &streng_width_D, 1, 5);
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("unit5"))
    {
        UnitImGui(unit_E);
        ImGui::SliderFloat(u8"�U����", &attack_width_E, 0.1f, 10.0f);
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("unit6"))
    {
        UnitImGui(unit_F);
        ImGui::SliderFloat(u8"�U����", &attack_width_F, 0.1f, 10.0f);
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
    unit->SetRadius(radius_D);
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
