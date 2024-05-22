#include "Unit_HIJ.h"
#include "EnemyManager.h"
#include "Lemur/Audio/AudioManager.h"

Unit_J::Unit_J()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    //LoadGltfModel(graphics.GetDevice(), ".\\resources\\Model_glb\\Unit\\Chili.glb",true);
    LoadFBXModel(graphics.GetDevice(), ".\\resources\\Model\\Unit\\Mustard.fbx");

    collision_model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Collision\\donut_collision.fbx");

    attack_effect = new Effect(".\\resources\\Effect\\UNIT1\\UNIT1.efk");
    death_effect = new Effect(".\\resources\\Effect\\UNIT_DEATH\\UNIT_DEATH.efk");
    set_effect = new Effect(".\\resources\\Effect\\UNIT_SET\\UNIT_SET.efk");

    attack_times = 5;    // �U����
    attack_power = 1;    // �U����
    attack_interval = 0.5f; // �U���Ԋu
    attack_collision_range = 2.0f; // �U���͈�
    attack_radius_in = 1.0f; // �U���͈�
    radius = 0.4f; // ���a
    height = 0.5f; // �f�o�b�O�p
    dec_pos = 1.0f; // ���j�b�g�ɐڐG�����킪�ǂ̂��炢���˕Ԃ���邩

    UpdateTransform();

    // �Ƃ肠�����A�j���[�V����
    PlayAnimation(Animation::Idle, true);
    Lemur::Audio::AudioManager::Instance().PlaySe(Lemur::Audio::SE::APPEARANCE, false);
}

void Unit_J::DrawDebugPrimitive()
{
    DebugRenderer* debug_renderer = Lemur::Graphics::Graphics::Instance().GetDebugRenderer();
 /*   debug_renderer->DrawCylinder(position, attack_collision_range, height, { 1,0,1,1 });
    debug_renderer->DrawCylinder(position, attack_radius_in, height, { 1,0,1,1 });
    debug_renderer->DrawCylinder(position, radius, height, { 0,1,0,1 });*/
}

void Unit_J::UpdateIdleState(float elapsed_time)
{
    EnemyManager& enemyManager = EnemyManager::Instance();

    int enemyCount = enemyManager.GetEnemyCount();

    //TODO ���[�V������������܂��ς���
    // �G�̑�������
    for (int j = 0; j < enemyCount; ++j)
    {
        Enemy* enemy = enemyManager.GetEnemy(j);

        // ���j�b�g������ł���R���e�B�j���[
        if (enemy->IsDead())continue;

        // �G�����j�b�g�̍U���͈͂ɓ����Ă���Ƃ�
        if (Collision::IntersectDonutVsCircle
        (
            { position.x,position.z },                          // ���j�b�g�̈ʒu(XZ����)
            attack_collision_range,    
            attack_radius_in,
            { enemy->GetPosition().x,enemy->GetPosition().z },  // �G�̈ʒu(XZ����)
            enemy->GetRadius()                                  // �G�̓����蔻��
        ))
        {
            // �U���X�e�[�g�ɐ؂�ւ�
            TransitionAttackState();
        }
    }
    // �U���񐔂���������������
    if (attack_times <= 0)
    {
        TransitionDeathState();
    }
}

void Unit_J::UpdateAttackState(float elapsed_time)
{
    EnemyManager& enemyManager = EnemyManager::Instance();
    int enemyCount = enemyManager.GetEnemyCount();
    bool is_intersected = false;

    attack_timer += elapsed_time;// �U���^�C�}�[
    // �^�C�}�[���K�莞�Ԃ𒴂�����U��
    if (attack_timer >= attack_interval)is_attack = true;

    if (attack_times > 0)// �U���񐔂��c���Ă���Ƃ�
    {
        // �G�̑�������
        for (int j = 0; j < enemyCount; ++j)
        {
            Enemy* enemy = enemyManager.GetEnemy(j);

            // ���j�b�g������ł���R���e�B�j���[
            if (enemy->IsDead())continue;

            // ���j�b�g�̍U���͈͂ɓ����Ă���G�S���ɍU��
            if (Collision::IntersectDonutVsCircle
            (
                { position.x,position.z },                          // ���j�b�g�̈ʒu(XZ����)
                attack_collision_range,
                attack_radius_in,
                { enemy->GetPosition().x,enemy->GetPosition().z },  // �G�̈ʒu(XZ����)
                enemy->GetRadius()                                  // �G�̓����蔻��
            ))
            {
                // �G�Ƃ��Ԃ����t���O��ON
                is_intersected = true;

                if (is_attack)
                {
                    Lemur::Audio::AudioManager::Instance().PlaySe(Lemur::Audio::SE::UNIT_ATTACK, false);
                    // �A�j���[�V�����̐؂�ւ�
                    PlayAnimation(Animation::Attack, false);
                    // �U���t���O��ON�Ȃ�_���[�W����
                    enemy->ApplyDamage(ReturnDamage());
                    // �G�t�F�N�g�̍Đ�
                    attack_handle = attack_effect->Play(position, attack_effect_size);
                }
            }
        }

        // �͈͓��ɓG����̂����Ȃ���Αҋ@
        if (!is_intersected)    TransitionIdleState();
        else// �N����̂ł��͈͓��ɂ���ꍇ
        {
            // �U�����Ȃ�c��U���񐔂����炵�^�C�}�[��������
            if (is_attack)
            {
                is_attack = false;
                attack_timer = 0.0f;
                attack_times--;
            }
        }
    }
    else
    {
        // �U���񐔂���������������
        if (!IsPlayAnimation()) TransitionDeathState();
    }
}

void Unit_J::DrawDebugGUI(int n)
{
    Character::DrawDebugGUI("Unit_J", n);
}
