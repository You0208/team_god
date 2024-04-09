#include "Unit_A.h"
#include "EnemyManager.h"
#include "UnitManager.h"
#include "Lemur/Input/Input.h"
#include "Lemur/Graphics/DebugRenderer.h"
#include "Lemur/Collision/Collision.h"
#include "interval.h"

Unit_A::Unit_A()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Unit\\Chili_24_0305_01.fbx");

    attack_times = 5;// �U����
    attack_power = 1;// �U����
    attack_interval = 0.5f;// �U���Ԋu
    attack_radius = 1.0f;// �U���͈�
    radius = 0.3f;// ���a
    height = 0.5f;// �f�o�b�O�p
    dec_pos = 1.0f;// 

    // �Ƃ肠�����A�j���[�V����
    model->PlayAnimation(0, true);
}

Unit_A::~Unit_A()
{
}

void Unit_A::Update(float elapsedTime)
{
    // ���͏����X�V
    UpdateVelocity(elapsedTime);

    // ���I���ԍX�V
    UpdateInvincibleTimer(elapsedTime);

    // Transform �̍X�V
    UpdateTransform();

    // ���f���A�j���[�V�����X�V����
    model->UpdateAnimation(elapsedTime);

    DrawDebugGUI();

    AttackEnemy(elapsedTime);
}

void Unit_A::Render(float scale, ID3D11PixelShader** replaced_pixel_shader)
{
    model->Render(scale, replaced_pixel_shader);
}

void Unit_A::AttackEnemy(float elapsedTime)
{
    EnemyManager& enemyManager = EnemyManager::Instance();

    int enemyCount = enemyManager.GetEnemyCount();

    //TODO ���[�V������������܂��ς���
    // �G�̑�������
    for (int j = 0; j < enemyCount; ++j)
    {
        Enemy* enemy = enemyManager.GetEnemy(j);

        // �G�����j�b�g�̍U���͈͂ɓ����Ă���Ƃ�
        if (Collision::IntersectCircleVsCircle
        (
            {position.x,position.z},                // ���j�b�g�̈ʒu(XZ����)
            attack_radius,                          // �U���͈�
            {enemy->position.x,enemy->position.z},  // �G�̈ʒu(XZ����)
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

void Unit_A::DrawDebugGUI()
{
    //TODO ������ޕ�������Ƃ�΂�
    model->DrawDebug("Unit_A");
}

void Unit_A::DrawDebugPrimitive()
{
    DebugRenderer* debug_renderer = Lemur::Graphics::Graphics::Instance().GetDebugRenderer();
    debug_renderer->DrawCylinder(position, attack_radius, height, { 1,0,0,1 });
    debug_renderer->DrawCylinder(position, radius, height, { 0,1,0,1 });
}

