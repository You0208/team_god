#include "Unit_B.h"
#include "EnemyManager.h"
#include "UnitManager.h"
#include "Lemur/Input/Input.h"
#include "Lemur/Graphics/DebugRenderer.h"
#include "Lemur/Collision/Collision.h"

Unit_B::Unit_B()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Unit\\Chili_24_0305_01.fbx");

    attack_times    = 5;    // �U����
    attack_power    = 1;    // �U����
    attack_interval = 0.5f; // �U���Ԋu
    attack_radius   = 1.0f; // �U���͈�
    radius          = 0.3f; // ���a
    height          = 0.5f; // �f�o�b�O�p
    dec_pos         = 1.0f; // ���j�b�g�ɐڐG�����킪�ǂ̂��炢���˕Ԃ���邩

    // �O�p
    t_height = 1.0f;// ����
    t_base = 1.0f;// ���

    // ���_�����j�b�g�̃|�W�V������
    triangle_1.A = triangle_2.A = { position.x,position.z };
    // �������̒��_
    triangle_1.B = { triangle_1.A.x - t_height,triangle_1.A.y + (t_base * 0.5f) };
    triangle_2.B = { triangle_2.A.x + t_height,triangle_2.A.y + (t_base * 0.5f) };
    // �E��O���̒��_
    triangle_1.C = { triangle_1.A.x - t_height,triangle_1.A.y - (t_base * 0.5f) };
    triangle_2.C = { triangle_2.A.x + t_height,triangle_2.A.y - (t_base * 0.5f) };


    // �Ƃ肠�����A�j���[�V����
    model->PlayAnimation(0, true);
}

Unit_B::~Unit_B()
{
}

void Unit_B::Update(float elapsedTime)
{
    // ���_�����j�b�g�̃|�W�V������
    triangle_1.A = triangle_2.A = { position.x,position.z };
    // �������̒��_
    triangle_1.B = { triangle_1.A.x - t_height,triangle_1.A.y + (t_base * 0.5f) };
    triangle_2.B = { triangle_2.A.x + t_height,triangle_2.A.y + (t_base * 0.5f) };
    // �E��O���̒��_
    triangle_1.C = { triangle_1.A.x - t_height,triangle_1.A.y - (t_base * 0.5f) };
    triangle_2.C = { triangle_2.A.x + t_height,triangle_2.A.y - (t_base * 0.5f) };

    // ���͏����X�V
    UpdateVelocity(elapsedTime);

    // ���I���ԍX�V
    UpdateInvincibleTimer(elapsedTime);

    // Transform �̍X�V
    UpdateTransform();

    // ���f���A�j���[�V�����X�V����
    model->UpdateAnimation(elapsedTime);

    DrawDebugPrimitive();
    DrawDebugGUI();

    AttackEnemy(elapsedTime);
}

void Unit_B::Render(float scale, ID3D11PixelShader** replaced_pixel_shader)
{
    model->Render(scale, replaced_pixel_shader);
}

void Unit_B::AttackEnemy(float elapsedTime)
{
    EnemyManager& enemyManager = EnemyManager::Instance();

    int enemyCount = enemyManager.GetEnemyCount();

    //TODO ���[�V������������܂��ς���
    // �G�̑�������
    for (int j = 0; j < enemyCount; ++j)
    {
        Enemy* enemy = enemyManager.GetEnemy(j);

        // �G�����j�b�g�̍U���͈͂ɓ����Ă���Ƃ�
        // ���O�p
        if (Collision::IntersectTriangleVsCircle
        (
            triangle_1,
            { enemy->position.x,enemy->position.z },  // �G�̈ʒu(XZ����)
            enemy->radius                           // �G�̓����蔻��
        ))
        {
            attack_timer += elapsedTime;

            if (attack_timer >= attack_interval)
            {
                if (enemy->ApplyDamage(attack_power))
                {
                    attack_timer = 0.0f;
                    attack_times--;
                }
            }
        }
        // �E�O�p
        if (Collision::IntersectTriangleVsCircle
        (
            triangle_2,
            { enemy->position.x,enemy->position.z },  // �G�̈ʒu(XZ����)
            enemy->radius                           // �G�̓����蔻��
        ))
        {
            attack_timer += elapsedTime;

            if (attack_timer >= attack_interval)
            {
                if (enemy->ApplyDamage(attack_power))
                {
                    attack_timer = 0.0f;
                    attack_times--;
                }
            }
        }
    }

    // �U���񐔂���������������
    if (attack_times <= 0)
    {
        UnitManager::Instance().Remove(this);
    }
}

void Unit_B::DrawDebugGUI()
{
    std::string name = "Unit_B";

    std::string T = std::string("Transform") + name;
    if (ImGui::TreeNode(T.c_str()))
    {
        std::string spe = std::string("t_height") + name;
        ImGui::SliderFloat(spe.c_str(), &t_height, 5.0f, 0.0f);
        std::string pe = std::string("t_base") + name;
        ImGui::SliderFloat(pe.c_str(), &t_base, 5.0f, 0.0f);

        std::string p = std::string("position") + name;
        ImGui::DragFloat3(p.c_str(), &position.x, 1.0f, -FLT_MAX, FLT_MAX);
        std::string s = std::string("scale") + name;
        ImGui::DragFloat3(s.c_str(), &scale.x, 0.001f, -FLT_MAX, FLT_MAX);

        std::string r = std::string("rotation") + name;
        DirectX::XMFLOAT3 rot{};
        rot.x = DirectX::XMConvertToDegrees(rotation.x);
        rot.y = DirectX::XMConvertToDegrees(rotation.y);
        rot.z = DirectX::XMConvertToDegrees(rotation.z);
        ImGui::DragFloat3(r.c_str(), &rot.x, 0.5f, -FLT_MAX, FLT_MAX);
        rotation.x = DirectX::XMConvertToRadians(rot.x);
        rotation.y = DirectX::XMConvertToRadians(rot.y);
        rotation.z = DirectX::XMConvertToRadians(rot.z);

        std::string s_f = std::string("scale_facter") + name;
        ImGui::DragFloat(s_f.c_str(), &scaleFactor, 0.001f, 0.001f, 1.0f);
        ImGui::TreePop();
    }
}

void Unit_B::DrawDebugPrimitive()
{
    DebugRenderer* debug_renderer = Lemur::Graphics::Graphics::Instance().GetDebugRenderer();
    debug_renderer->DrawCylinder(position, radius, height, { 0,1,0,1 });

    // ���O�p
    debug_renderer->DrawSphere({ triangle_1.A.x,0.2f,triangle_1.A.y }, 0.1f, { 1,0,1,1 });
    debug_renderer->DrawSphere({ triangle_1.B.x,0.2f,triangle_1.B.y }, 0.1f, { 1,0,1,1 });
    debug_renderer->DrawSphere({ triangle_1.C.x,0.2f,triangle_1.C.y }, 0.1f, { 1,0,1,1 });

    // �E�O�p
    debug_renderer->DrawSphere({ triangle_2.A.x,0.2f,triangle_2.A.y }, 0.1f, { 0,0,1,1 });
    debug_renderer->DrawSphere({ triangle_2.B.x,0.2f,triangle_2.B.y }, 0.1f, { 0,0,1,1 });
    debug_renderer->DrawSphere({ triangle_2.C.x,0.2f,triangle_2.C.y }, 0.1f, { 0,0,1,1 });
}