#include "Unit_HIJ.h"
#include "EnemyManager.h"


Unit_H::Unit_H()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Unit\\unit3_RE.fbx");

    attack_times = 5;    // �U����
    attack_power = 1;    // �U����
    attack_interval = 0.5f; // �U���Ԋu
    attack_collision_range = 1.0f; // �U���͈�
    radius = 0.3f; // ���a
    height = 0.5f; // �f�o�b�O�p
    dec_pos = 1.0f; // ���j�b�g�ɐڐG�����킪�ǂ̂��炢���˕Ԃ���邩
    timer_max = 1.0f; // �o�t�̕b��
    rect_angle = 50.0f; // ��`�p�x
    attack_rect.center = { position.x ,position.z };
    attack_rect.width = { 1.0f ,20.0f };
    attack_rect = CalcRotateRect(attack_rect.center, attack_rect.width, rect_angle);

    // �Ƃ肠�����A�j���[�V����
    model->PlayAnimation(Animation::Idle, true);
}

void Unit_H::DrawDebugPrimitive()
{
    DebugRenderer* debug_renderer = Lemur::Graphics::Graphics::Instance().GetDebugRenderer();
    debug_renderer->DrawBox({ attack_rect.center.x,0, attack_rect.center.y },
        { 0,DirectX::XMConvertToRadians(rect_angle),0 },
        { attack_rect.width.x*0.5f,0.5f, attack_rect.width.y*0.5f }, { 1,0,1,1 });
    debug_renderer->DrawSphere({ attack_rect.right_down.x,0.5f, attack_rect.right_down.y }, 0.1f, { 0,0,1,1 });
    debug_renderer->DrawSphere({ attack_rect.left_up.x,0.5f, attack_rect.left_up.y }, 0.1f, { 0,1,0,1 });

}

void Unit_H::Update(float elapsed_time)
{
    attack_rect.center = { position.x ,position.z };
    // ��ɋ�`���X�V
    attack_rect = CalcRotateRect(attack_rect.center, attack_rect.width, rect_angle);

    Unit::Update(elapsed_time);
}

void Unit_H::UpdateIdleState(float elapsed_time)
{
    EnemyManager& enemyManager = EnemyManager::Instance();
    int enemyCount = enemyManager.GetEnemyCount();

    //TODO ���[�V������������܂��ς���
    // �G�̑�������
    for (int j = 0; j < enemyCount; ++j)
    {
        Enemy* enemy = enemyManager.GetEnemy(j);

        // �G�����j�b�g�̍U���͈͂ɓ����Ă���Ƃ�
        if (Collision::IntersectRotateRectVsCircle(
            attack_rect,
            { enemy->GetPosition().x, enemy->GetPosition().z },
            enemy->GetRadius(),
            rect_angle))
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

void Unit_H::UpdateAttackState(float elapsed_time)
{
    EnemyManager& enemyManager = EnemyManager::Instance();
    int enemyCount = enemyManager.GetEnemyCount();
    bool is_intersected = false;

    attack_timer += elapsed_time;// �U���^�C�}�[
    // �^�C�}�[���K�莞�Ԃ𒴂�����U��
    if (attack_timer >= attack_interval)is_attack = true;

    //TODO ���[�V������������܂��ς���
    // �G�̑�������
    for (int j = 0; j < enemyCount; ++j)
    {
        Enemy* enemy = enemyManager.GetEnemy(j);

        // ���j�b�g�̍U���͈͂ɓ����Ă���G�S���ɍU��
        if (Collision::IntersectRotateRectVsCircle(
            attack_rect,
            { enemy->GetPosition().x, enemy->GetPosition().z },
            enemy->GetRadius(),
            rect_angle))
        {
            int damage = ReturnDamage();
            is_intersected = true;
            if (is_attack)  enemy->ApplyDamage(damage);
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

    // �U���񐔂���������������
    if (attack_times <= 0)
    {
        TransitionDeathState();
    }
}

void Unit_H::DrawDebugGUI(int n)
{
    Character::DrawDebugGUI("Unit_H", n);
}

Unit_I::Unit_I()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Unit\\unit3_RE.fbx");

    attack_times = 5;    // �U����
    attack_power = 1;    // �U����
    attack_interval = 0.5f; // �U���Ԋu
    attack_collision_range = 1.0f; // �U���͈�
    radius = 0.3f; // ���a
    height = 0.5f; // �f�o�b�O�p
    dec_pos = 1.0f; // ���j�b�g�ɐڐG�����킪�ǂ̂��炢���˕Ԃ���邩
    timer_max = 1.0f; // �o�t�̕b��
    rect_angle = 50.0f; // ��`�p�x
    attack_rect.center = { position.x ,position.z };
    attack_rect.width = { 1.0f ,20.0f };
    attack_rect = CalcRotateRect(attack_rect.center, attack_rect.width, rect_angle);

    // �Ƃ肠�����A�j���[�V����
    model->PlayAnimation(Animation::Idle, true);
}

void Unit_I::DrawDebugPrimitive()
{
    DebugRenderer* debug_renderer = Lemur::Graphics::Graphics::Instance().GetDebugRenderer();
    debug_renderer->DrawBox({ attack_rect.center.x,0, attack_rect.center.y },
        { 0,DirectX::XMConvertToRadians(rect_angle),0 },
        { attack_rect.width.x * 0.5f,0.5f, attack_rect.width.y * 0.5f }, { 1,0,1,1 });
    debug_renderer->DrawSphere({ attack_rect.right_down.x,0.5f, attack_rect.right_down.y }, 0.1f, { 0,0,1,1 });
    debug_renderer->DrawSphere({ attack_rect.left_up.x,0.5f, attack_rect.left_up.y }, 0.1f, { 0,1,0,1 });

}

void Unit_I::Update(float elapsed_time)
{
    attack_rect.center = { position.x ,position.z };
    // ��ɋ�`���X�V
    attack_rect = CalcRotateRect(attack_rect.center, attack_rect.width, rect_angle);

    Unit::Update(elapsed_time);
}

void Unit_I::UpdateIdleState(float elapsed_time)
{
    EnemyManager& enemyManager = EnemyManager::Instance();
    int enemyCount = enemyManager.GetEnemyCount();

    //TODO ���[�V������������܂��ς���
    // �G�̑�������
    for (int j = 0; j < enemyCount; ++j)
    {
        Enemy* enemy = enemyManager.GetEnemy(j);

        // �G�����j�b�g�̍U���͈͂ɓ����Ă���Ƃ�
        if (Collision::IntersectRotateRectVsCircle(
            attack_rect,
            { enemy->GetPosition().x, enemy->GetPosition().z },
            enemy->GetRadius(),
            rect_angle))
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

void Unit_I::UpdateAttackState(float elapsed_time)
{
    EnemyManager& enemyManager = EnemyManager::Instance();
    int enemyCount = enemyManager.GetEnemyCount();
    bool is_intersected = false;

    attack_timer += elapsed_time;// �U���^�C�}�[
    // �^�C�}�[���K�莞�Ԃ𒴂�����U��
    if (attack_timer >= attack_interval)is_attack = true;

    //TODO ���[�V������������܂��ς���
    // �G�̑�������
    for (int j = 0; j < enemyCount; ++j)
    {
        Enemy* enemy = enemyManager.GetEnemy(j);

        // ���j�b�g�̍U���͈͂ɓ����Ă���G�S���ɍU��
        if (Collision::IntersectRotateRectVsCircle(
            attack_rect,
            { enemy->GetPosition().x, enemy->GetPosition().z },
            enemy->GetRadius(),
            rect_angle))
        {
            int damage = ReturnDamage();
            is_intersected = true;
            if (is_attack)  enemy->ApplyDamage(damage);
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

    // �U���񐔂���������������
    if (attack_times <= 0)
    {
        TransitionDeathState();
    }
}

void Unit_I::DrawDebugGUI(int n)
{
    Character::DrawDebugGUI("Unit_I", n);
}


Unit_J::Unit_J()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Unit\\unit1_RE.fbx");

    attack_times = 5;    // �U����
    attack_power = 1;    // �U����
    attack_interval = 0.5f; // �U���Ԋu
    attack_collision_range = 2.0f; // �U���͈�
    attack_radius_in = 1.0f; // �U���͈�
    radius = 0.3f; // ���a
    height = 0.5f; // �f�o�b�O�p
    dec_pos = 1.0f; // ���j�b�g�ɐڐG�����킪�ǂ̂��炢���˕Ԃ���邩

    UpdateTransform();

    // �Ƃ肠�����A�j���[�V����
    model->PlayAnimation(Animation::Idle, true);
}

void Unit_J::DrawDebugPrimitive()
{
    DebugRenderer* debug_renderer = Lemur::Graphics::Graphics::Instance().GetDebugRenderer();
    debug_renderer->DrawCylinder(position, attack_collision_range, height, { 1,0,1,1 });
    debug_renderer->DrawCylinder(position, attack_radius_in, height, { 1,0,1,1 });
    debug_renderer->DrawCylinder(position, radius, height, { 0,1,0,1 });
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

    //TODO ���[�V������������܂��ς���
    // �G�̑�������
    for (int j = 0; j < enemyCount; ++j)
    {
        Enemy* enemy = enemyManager.GetEnemy(j);

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
            is_intersected = true;
            if (is_attack)  enemy->ApplyDamage(ReturnDamage());
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

    // �U���񐔂���������������
    if (attack_times <= 0)
    {
        TransitionDeathState();
    }
}

void Unit_J::DrawDebugGUI(int n)
{
    Character::DrawDebugGUI("Unit_J", n);
}
