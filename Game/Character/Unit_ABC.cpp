#include "Unit_ABC.h"
#include "EnemyManager.h"

//--------Unit_A--------------------------------------------------
Unit_A::Unit_A()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Unit\\unit1_RE.fbx");

    attack_times           = 5;    // �U����
    attack_power           = 1;    // �U����
    attack_interval        = 0.5f; // �U���Ԋu
    attack_collision_range = 1.0f; // �U���͈�
    radius                 = 0.3f; // ���a
    height                 = 0.5f; // �f�o�b�O�p
    dec_pos                = 1.0f; // ���j�b�g�ɐڐG�����킪�ǂ̂��炢���˕Ԃ���邩

    UpdateTransform();

    // �Ƃ肠�����A�j���[�V����
    model->PlayAnimation(Animation::Idle, true);
}

void Unit_A::DrawDebugPrimitive()
{
    DebugRenderer* debug_renderer = Lemur::Graphics::Graphics::Instance().GetDebugRenderer();
    debug_renderer->DrawCylinder(position, attack_collision_range, height, { 1,0,0,1 });
    debug_renderer->DrawCylinder(position, radius, height, { 0,1,0,1 });
}

void Unit_A::UpdateIdleState(float elapsed_time)
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
            { position.x,position.z },                          // ���j�b�g�̈ʒu(XZ����)
            attack_collision_range,                             // �U���͈�
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

void Unit_A::UpdateAttackState(float elapsed_time)
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
        if (Collision::IntersectCircleVsCircle
        (
            { position.x,position.z },                          // ���j�b�g�̈ʒu(XZ����)
            attack_collision_range,                             // �U���͈�
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

void Unit_A::DrawDebugGUI(int n)
{
    Character::DrawDebugGUI("Unit_A", n);
}


//--------Unit_B--------------------------------------------------

Unit_B::Unit_B()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Unit\\unit2_RE.fbx");

    attack_times           = 5;    // �U����
    attack_power           = 1;    // �U����
    attack_interval        = 0.5f; // �U���Ԋu
    attack_collision_range = 1.0f; // �U���͈�
    radius                 = 0.3f; // ���a
    height                 = 0.5f; // �f�o�b�O�p
    dec_pos                = 1.0f; // ���j�b�g�ɐڐG�����킪�ǂ̂��炢���˕Ԃ���邩

    // �O�p
    t_height               = 1.0f;// ����
    t_base                 = 1.0f;// ���

    // ���_�����j�b�g�̃|�W�V������
    triangle_1.A           = triangle_2.A = { position.x,position.z };
    // �������̒��_
    triangle_1.B           = { triangle_1.A.x - t_height,triangle_1.A.y + (t_base * 0.5f) };
    triangle_2.B           = { triangle_2.A.x + t_height,triangle_2.A.y + (t_base * 0.5f) };
    // �E��O���̒��_
    triangle_1.C           = { triangle_1.A.x - t_height,triangle_1.A.y - (t_base * 0.5f) };
    triangle_2.C           = { triangle_2.A.x + t_height,triangle_2.A.y - (t_base * 0.5f) };

    UpdateTransform();


    // �Ƃ肠�����A�j���[�V����
    model->PlayAnimation(Animation::Idle, true);
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

void Unit_B::Update(float elapsed_time)
{
    // ���_�����j�b�g�̃|�W�V������
    triangle_1.A = triangle_2.A = { position.x,position.z };
    // �������̒��_
    triangle_1.B = { triangle_1.A.x - t_height,triangle_1.A.y + (t_base * 0.5f) };
    triangle_2.B = { triangle_2.A.x + t_height,triangle_2.A.y + (t_base * 0.5f) };
    // �E��O���̒��_
    triangle_1.C = { triangle_1.A.x - t_height,triangle_1.A.y - (t_base * 0.5f) };
    triangle_2.C = { triangle_2.A.x + t_height,triangle_2.A.y - (t_base * 0.5f) };


    Unit::Update(elapsed_time);
}

void Unit_B::UpdateIdleState(float elapsed_time)
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
            { enemy->GetPosition().x,enemy->GetPosition().z },  // �G�̈ʒu(XZ����)
            enemy->GetRadius()                           // �G�̓����蔻��
        ))
        {
            TransitionAttackState();
        }
        // �E�O�p
        if (Collision::IntersectTriangleVsCircle
        (
            triangle_2,
            { enemy->GetPosition().x,enemy->GetPosition().z },  // �G�̈ʒu(XZ����)
            enemy->GetRadius()                           // �G�̓����蔻��
        ))
        {
            TransitionAttackState();
        }
    }

    // �U���񐔂���������������
    if (attack_times <= 0)
    {
        TransitionIdleState();
    }
}

void Unit_B::UpdateAttackState(float elapsed_time)
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

        // �G�����j�b�g�̍U���͈͂ɓ����Ă���Ƃ�
        // ���O�p
        if (Collision::IntersectTriangleVsCircle
        (
            triangle_1,
            { enemy->GetPosition().x,enemy->GetPosition().z },  // �G�̈ʒu(XZ����)
            enemy->GetRadius()                           // �G�̓����蔻��
        ))
        {
            is_intersected = true;
            if (is_attack)  enemy->ApplyDamage(ReturnDamage());
        }
        // �E�O�p
        if (Collision::IntersectTriangleVsCircle
        (
            triangle_2,
            { enemy->GetPosition().x,enemy->GetPosition().z },  // �G�̈ʒu(XZ����)
            enemy->GetRadius()                           // �G�̓����蔻��
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

void Unit_B::DrawDebugGUI(int n)
{
    Character::DrawDebugGUI("Unit_B", n);
}


//--------Unit_C--------------------------------------------------

Unit_C::Unit_C()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Unit\\unit3_RE.fbx");

    attack_times           = 5;    // �U����
    attack_power           = 1;    // �U����
    attack_interval        = 0.5f; // �U���Ԋu
    attack_collision_range = 1.0f; // �U���͈�
    radius                 = 0.3f; // ���a
    height                 = 0.5f; // �f�o�b�O�p
    dec_pos                = 1.0f; // ���j�b�g�ɐڐG�����킪�ǂ̂��炢���˕Ԃ���邩

    // �O�p
    t_height               = 1.0f;// ����
    t_base                 = 1.0f;// ���

    // triangle_1=���Atriangle_2=��O
    // ���_�����j�b�g�̃|�W�V������
    triangle_1.A           = triangle_2.A = { position.x,position.z };
    // �����̒��_
    triangle_1.B           = { triangle_1.A.x - (t_base * 0.5f),triangle_1.A.y + t_height };
    triangle_2.B           = { triangle_2.A.x - (t_base * 0.5f),triangle_2.A.y - t_height };
    // �E���̒��_
    triangle_1.C           = { triangle_1.A.x + (t_base * 0.5f),triangle_1.A.y + t_height };
    triangle_2.C           = { triangle_2.A.x + (t_base * 0.5f),triangle_2.A.y - t_height };


    UpdateTransform();

    // �Ƃ肠�����A�j���[�V����
    model->PlayAnimation(Animation::Idle, true);
}

void Unit_C::DrawDebugPrimitive()
{
    DebugRenderer* debug_renderer = Lemur::Graphics::Graphics::Instance().GetDebugRenderer();
    debug_renderer->DrawCylinder(position, radius, height, { 0,1,0,1 });

    // ���O�p
    debug_renderer->DrawSphere({ triangle_1.A.x,0.2f,triangle_1.A.y }, 0.1f, { 1,0,1,1 });
    debug_renderer->DrawSphere({ triangle_1.B.x,0.2f,triangle_1.B.y }, 0.1f, { 1,0,1,1 });
    debug_renderer->DrawSphere({ triangle_1.C.x,0.2f,triangle_1.C.y }, 0.1f, { 1,0,1,1 });

    // ��O�O�p
    debug_renderer->DrawSphere({ triangle_2.A.x,0.2f,triangle_2.A.y }, 0.1f, { 0,0,1,1 });
    debug_renderer->DrawSphere({ triangle_2.B.x,0.2f,triangle_2.B.y }, 0.1f, { 0,0,1,1 });
    debug_renderer->DrawSphere({ triangle_2.C.x,0.2f,triangle_2.C.y }, 0.1f, { 0,0,1,1 });
}

void Unit_C::Update(float elapsed_time)
{
    // triangle_1   =���Atriangle_2=��O
    // ���_�����j�b�g�̃|�W�V������
    triangle_1.A = triangle_2.A = { position.x,position.z };
    // �����̒��_
    triangle_1.B = { triangle_1.A.x - (t_base * 0.5f),triangle_1.A.y + t_height };
    triangle_2.B = { triangle_2.A.x - (t_base * 0.5f),triangle_2.A.y - t_height };
    // �E���̒��_
    triangle_1.C = { triangle_1.A.x + (t_base * 0.5f),triangle_1.A.y + t_height };
    triangle_2.C = { triangle_2.A.x + (t_base * 0.5f),triangle_2.A.y - t_height };

    Unit::Update(elapsed_time);
}

void Unit_C::UpdateIdleState(float elapsed_time)
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
            { enemy->GetPosition().x,enemy->GetPosition().z },  // �G�̈ʒu(XZ����)
            enemy->GetRadius()                           // �G�̓����蔻��
        ))
        {
            TransitionAttackState();
        }
        // ��O�O�p
        if (Collision::IntersectTriangleVsCircle
        (
            triangle_2,
            { enemy->GetPosition().x,enemy->GetPosition().z },  // �G�̈ʒu(XZ����)
            enemy->GetRadius()                           // �G�̓����蔻��
        ))
        {
            TransitionAttackState();
        }
    }

    // �U���񐔂���������������
    if (attack_times <= 0)
    {
        TransitionIdleState();
    }
}

void Unit_C::UpdateAttackState(float elapsed_time)
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

        // �G�����j�b�g�̍U���͈͂ɓ����Ă���Ƃ�
        // ���O�p
        if (Collision::IntersectTriangleVsCircle
        (
            triangle_1,
            { enemy->GetPosition().x,enemy->GetPosition().z },  // �G�̈ʒu(XZ����)
            enemy->GetRadius()                           // �G�̓����蔻��
        ))
        {
            is_intersected = true;
            if (is_attack)  enemy->ApplyDamage(ReturnDamage());
        }
        // �E�O�p
        if (Collision::IntersectTriangleVsCircle
        (
            triangle_2,
            { enemy->GetPosition().x,enemy->GetPosition().z },  // �G�̈ʒu(XZ����)
            enemy->GetRadius()                           // �G�̓����蔻��
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
void Unit_C::DrawDebugGUI(int n)
{
    Character::DrawDebugGUI("Unit_C", n);
}
