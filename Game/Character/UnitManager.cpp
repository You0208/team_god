#include "UnitManager.h"
#include "Lemur/Graphics/Graphics.h"
#include "Lemur/Collision/Collision.h"

void UnitManager::Initialize()
{
    //TODO ���ˁ@���j�b�g�X�e�[�^�X
    {
        //Unit_A (��)
        unit_A.attack_interval = 1.0f;    // �U���Ԋu
        unit_A.attack_power    = 3;       // �U����
        unit_A.attack_times    = 1;       // �U����
        radius_A               = 1.5f;    // �U�����a

        //Unit_B�i�O�p���j
        unit_B.attack_interval = 1.0f;  // �U���Ԋu
        unit_B.attack_power = 4;        // �U����
        unit_B.attack_times = 1;        // �U����
        t_height_B = 1.59f;             // �O�p�`�̍���
        t_base_B = 1.59f;               // �O�p�`�̒�Ӓ�

        //Unit_C�i�O�p�c�j
        unit_C.attack_interval = 1.0f;  // �U���Ԋu
        unit_C.attack_power = 4;        // �U����
        unit_C.attack_times = 1;        // �U����
        t_height_C = 1.59f;              // �O�p�`�̍���
        t_base_C = 1.59f;                // �O�p�`�̒�Ӓ�

        //Unit_D�i�o�t�j
        radius_D               = 3.0f; // �U�����a
        timer_max_D            = 5.0f; // �o������
        streng_width_D         = 2;    // �U���͂̏�蕝

        //Unit_E�i�������j
        unit_E.attack_interval = 1.0f;  // �U���Ԋu
        unit_E.attack_power    = 1;     // �U����
        unit_E.attack_times    = 1;     // �U����
        attack_width_E         = 0.9f;  // �U���͈͂̕�

        //Unit_F�i�����c�j
        unit_F.attack_interval = 1.0f; // �U���Ԋu
        unit_F.attack_power    = 1;    // �U����
        unit_F.attack_times    = 1;    // �U����
        attack_width_F         = 0.9f; // �U���͈͂̕�

        //Unit_H�i�E�΂߁j
        unit_H.attack_interval = 1.0f;        // �U���Ԋu
        unit_H.attack_power    = 1;           // �U����
        unit_H.attack_times    = 1;           // �U����
        attack_width_H         = {1.5f,20.0f};// �U���͈͂̕�
        rect_angle_H           = 50.0f;       // �U���͈͂̕�

        //Unit_I�i���΂߁j
        unit_I.attack_interval = 1.0f;          // �U���Ԋu
        unit_I.attack_power    = 1;             // �U����
        unit_I.attack_times    = 1;             // �U����
        attack_width_I         = { 1.5f,20.0f };// �U���͈͂̕�
        rect_angle_I           = -50.0f;        // �U���͈͂̕�

        //Unit_J(�h�[�i�c)
        unit_J.attack_interval = 1.0f; // �U���Ԋu
        unit_J.attack_power    = 1;    // �U����
        unit_J.attack_times    = 1;    // �U����
        radius_in_J            =1.0f;  // ���a
        radius_out_J           =1.7f;  // ���a

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
    if (ImGui::TreeNode("unit7"))
    {
        UnitImGui(unit_H);
        ImGui::SliderFloat2(u8"�U����", &attack_width_H.x, 0.1f, 20.0f);
        ImGui::SliderFloat(u8"�U���p�x", &rect_angle_H, 0.0f, 360.0f);
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("unit8"))
    {
        UnitImGui(unit_I);
        ImGui::SliderFloat2(u8"�U����", &attack_width_I.x, 0.1f, 20.0f);
        ImGui::SliderFloat(u8"�U���p�x", &rect_angle_I, 0.0f, 360.0f);
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("unit9"))
    {
        UnitImGui(unit_J);
        ImGui::SliderFloat(u8"�h�[�i�c�����a", &radius_in_J, 0.0f, 5.0f);
        ImGui::SliderFloat(u8"�h�[�i�c�O���a", &radius_out_J, 0.0f, 5.0f);
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
    unit->SetAttackRadius(radius_D);
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

void UnitManager::SetUpUnit_H(Unit* unit)
{
    unit->SetAttackTimes(unit_H.attack_times);
    unit->SetAttackPower(unit_H.attack_power);
    unit->SetAttackInterval(unit_H.attack_interval);
    unit->SetAttackWidth(attack_width_H);
    unit->SetRectAngle(rect_angle_H);
}

void UnitManager::SetUpUnit_I(Unit* unit)
{
    unit->SetAttackTimes(unit_I.attack_times);
    unit->SetAttackPower(unit_I.attack_power);
    unit->SetAttackInterval(unit_I.attack_interval);
    unit->SetAttackWidth(attack_width_I);
    unit->SetRectAngle(rect_angle_I);
}

void UnitManager::SetUpUnit_J(Unit* unit)
{
    unit->SetAttackTimes(unit_J.attack_times);
    unit->SetAttackPower(unit_J.attack_power);
    unit->SetAttackInterval(unit_J.attack_interval);
    unit->SetAttackRadius(radius_in_J);
    unit->SetAttackRadius(radius_out_J);
}
