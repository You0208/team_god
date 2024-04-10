#include "Enemy_ABCD.h"
#include "EnemyManager.h"
#include "UnitManager.h"

//------EnemyA-----------------------------------------------------------------

Enemy_A::Enemy_A()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Enemy\\spider_v009.fbx");

    attack_power = 1;     // �U����
    attack_interval = 3.0f;  // �U���Ԋu

    health = 10;    // HP
    radius = 1.0f;  // ���a
    height = 1.0f;  // �f�o�b�O�p
    position.x = 5.0f;  // �����ʒu
    rotation.y = -90.0f;// �����p�x
    speed_power = -1.0f; // ���x

    // �Ƃ肠�����A�j���[�V����
    model->PlayAnimation(Animation::Move, true);
}

void Enemy_A::DrawDebugPrimitive()
{
    DebugRenderer* debug_renderer = Lemur::Graphics::Graphics::Instance().GetDebugRenderer();
    debug_renderer->DrawCylinder(position, radius, height, { 0,1,0,1 });
}

void Enemy_A::DrawDebugGUI(int n)
{
    Character::DrawDebugGUI("Enemy_A", n);
}

void Enemy_A::UpdateAttackState(float elapsed_time)
{
    // �^�C�}�[�����Z
    timer += elapsed_time;
    // �U���Ԋu�ƃ^�C�}�[���ׂ�
    if (timer >= attack_interval)
    {
         // ��ɓ���������
        if (Fence::Instance().ApplyDamage(attack_power))
        {
            //TODO ����̓A�j���[�V������������v�ύX
            model->PlayAnimation(Animation::Attack, false);
            // �^�C�}�[����
            timer = 0.0f;
        }
    }

    //TODO ����̓A�j���[�V������������v�ύX
    if (!model->IsPlayAnimation())
    {
        model->PlayAnimation(7, true);
    }

    // ���S���͎��S�X�e�[�g�֐؂�ւ�
    if (IsDead())
    {
        TransitionDeathState();
    }
}

void Enemy_A::UpdateMoveState(float elapsed_time)
{
    // ��ɐG���΍U���X�e�[�g��
    if (Collision::IntersectRectVsCircle(Fence::Instance().left_rect, { position.x,position.z }, radius))
    {
        TransitionAttackState();
    }
    else
    {
        // �ړ�
        velocity.x = speed_power;
    }

    // ���S�X�e�[�g��
    if (IsDead())
    {
        TransitionDeathState();
    }
}


//------ EnemyB----------------------------------------------------------------

Enemy_B::Enemy_B()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Enemy\\spider_v009.fbx");

    attack_power = 1;        // �U����
    attack_interval = 3.0f;     // �U���Ԋu

    radius = 1.0f;     // ���a
    height = 1.0f;     // �f�o�b�O�p
    position.x = 5.0f;     // �����ʒu
    rotation.y = -90.0f;   // ��������

    dis = 3.0f;     // ��C�ɐi�ދ���
    move_timer = 0.0f;     // �^�C�}�[
    move_timer_max = 3.0f;     // �����܂ł̎���
    speed_power = -3.0f;    // �X�s�[�h

    health = 10;       // HP
    // �Ƃ肠�����A�j���[�V����
    model->PlayAnimation(Animation::Move, true);
}

void Enemy_B::DrawDebugGUI(int n)
{
    Character::DrawDebugGUI("Enemy_B", n);
}

void Enemy_B::UpdateAttackState(float elapsed_time)
{
    // �^�C�}�[�����Z
    attack_timer += elapsed_time;
    // �U���Ԋu�ƃ^�C�}�[���ׂ�
    if (attack_timer >= attack_interval)
    {
        // ��ɓ���������
        if (Fence::Instance().ApplyDamage(attack_power))
        {
            //TODO ����̓A�j���[�V������������v�ύX
            model->PlayAnimation(Animation::Attack, false);
            // �^�C�}�[����
            attack_timer = 0.0f;
        }
    }

    //TODO ����̓A�j���[�V������������v�ύX
    if (!model->IsPlayAnimation())
    {
        model->PlayAnimation(7, true);
    }

    // ���S���͎��S�X�e�[�g�֐؂�ւ�
    if (IsDead())
    {
        TransitionDeathState();
    }
}

void Enemy_B::UpdateMoveState(float elapsed_time)
{
    // �^�C�}�[�����Z
    move_timer += elapsed_time;

    if (move_timer >= move_timer_max)                   //�^�C�}�[��4�b�𒴂����Ƃ�
    {
        velocity.x = speed_power;                       // ���x����
        if (abs(old_position - position.x) >= dis)      // ����̋����i�񂾂�
        {
            move_timer = 0.0f;                          // �Ăёҋ@
        }
    }
    // ��ɓ���������
    else if (Collision::IntersectRectVsCircle(Fence::Instance().left_rect, { position.x,position.z }, radius))
    {
        TransitionAttackState();
    }
    else
    {
        // �O�ʒu��ύX
        old_position = position.x;
        // �ړ�
        velocity.x = 0.0f;
    }

    // ���S�X�e�[�g��
    if (IsDead())
    {
        TransitionDeathState();
    }
}

void Enemy_B::DrawDebugPrimitive()
{
    DebugRenderer* debug_renderer = Lemur::Graphics::Graphics::Instance().GetDebugRenderer();
    debug_renderer->DrawCylinder(position, radius, height, { 0,1,0,1 });
}

//------EnemyC-----------------------------------------------------------------

Enemy_C::Enemy_C()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Enemy\\spider_v009.fbx");

    attack_power = 1;         // �U���́@
    attack_interval = 3.0f;      // �U���Ԋu

    radius = 1.0f;      // ���a
    height = 1.0f;      // �f�o�b�O�v
    position.x = 5.0f;      // �����ʒu
    rotation.y = -90.0f;    // �����p�x
    speed_power = -1.0f;     // ���x
    health = 10;        // HP

    // �Ƃ肠�����A�j���[�V����
    model->PlayAnimation(Animation::Move, true);
}

void Enemy_C::DrawDebugPrimitive()
{
    DebugRenderer* debug_renderer = Lemur::Graphics::Graphics::Instance().GetDebugRenderer();
    debug_renderer->DrawCylinder(position, radius, height, { 0,1,0,1 });
}

void Enemy_C::DrawDebugGUI(int n)
{
    Character::DrawDebugGUI("Enemy_C", n);
}

void Enemy_C::UpdateAttackState(float elapsed_time)
{
    // �^�C�}�[�����Z
    timer += elapsed_time;
    // �U���Ԋu�ƃ^�C�}�[���ׂ�
    if (timer >= attack_interval)
    {
        // ��ɓ���������
        if (Fence::Instance().ApplyDamage(attack_power))
        {
            //TODO ����̓A�j���[�V������������v�ύX
            model->PlayAnimation(Animation::Attack, false);
            // �^�C�}�[����
            timer = 0.0f;
        }
    }

    //TODO ����̓A�j���[�V������������v�ύX
    if (!model->IsPlayAnimation())
    {
        model->PlayAnimation(7, true);
    }

    // ���S���͎��S�X�e�[�g�֐؂�ւ�
    if (IsDead())
    {
        TransitionDeathState();
    }
}

void Enemy_C::UpdateMoveState(float elapsed_time)
{
    // ��ɐG�ꂽ��
    if (Collision::IntersectRectVsCircle(Fence::Instance().left_rect, { position.x,position.z }, radius) ||
        Collision::IntersectRectVsCircle(Fence::Instance().front_rect, { position.x,position.z }, radius))
    {
        TransitionAttackState();
    }
    else
    {
        switch (move_state)
        {
        case 0:
            // �ړ�
            velocity.x = speed_power;
            if (is_touched_unit)move_state = 1;
            break;
        case 1:// ���j�b�g�ɐG������
            // ���ݒn����ړI�n�܂ł̃x�N�g�����Z�o
            DirectX::XMFLOAT2 Vec =
            {
                destination.x - position.x,
                destination.y - position.z
            };
            // ���K��
            Vec = Normalize(Vec);
            // �X�s�[�h�ɂ�����
            Vec = { Vec.x * abs(speed_power),Vec.y * abs(speed_power) };
            // �ړ�
            velocity.x = Vec.x;
            velocity.z = Vec.y;

            // �ړI�n�ɒ�������߂�
            if (position.z >= destination.y)
            {
                is_touched_unit = false;
                velocity.x = velocity.z = 0.0f;
                move_state = 0;
            }

            break;
        }
    }
}

void Enemy_C::JudgeUnit()
{
    UnitManager& unitManager = UnitManager::Instance();

    int unitCount = unitManager.GetUnitCount();

    // �G�̑�������
    for (int j = 0; j < unitCount; ++j)
    {
        Unit* unit = unitManager.GetUnit(j);

        // �G�����j�b�g�̍U���͈͂ɐG�ꂽ�Ƃ�
        if (Collision::IntersectCircleVsCircle
        (
            { unit->GetPosition().x,unit->GetPosition().z },   // ���j�b�g�̈ʒu(XZ����)
            unit->GetAttackRadius(),                 // �U���͈�
            { position.x,position.z },               // �G�̈ʒu(XZ����)
            radius                                   // �G�̓����蔻��
        ))
        {
            is_touched_unit = true;
            destination = {
                unit->GetPosition().x,
                unit->GetPosition().z + unit->GetRadius() + radius
            };
        }
    }
}

//-------EnemyD---------------------------------------------------------------

Enemy_D::Enemy_D()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Enemy\\spider_v009.fbx");

    attack_power = 1;        // �U����
    attack_interval = 3.0f;     // �U���Ԋu

    radius = 1.0f;     // ���a
    height = 1.0f;     //�f�o�b�O�p
    position.x = 5.0f;     // �����ʒu
    rotation.y = -90.0f;   // ��������

    move_state = 0;        // �ړ����
    speed_power = -1.0f;    // ���ړ���
    speed_power_Y = -1.0f;    // �c�ړ���
    direction_state = 0;        // �����̏��
    dis_max = 1.0f;     // �ړ��ʋK��l
    is_last_touched = false;    // �O���ɐG�ꂽ��

    health = 10;       // HP

    // �Ƃ肠�����A�j���[�V����
    model->PlayAnimation(Animation::Move, true);
}

void Enemy_D::DrawDebugPrimitive()
{
    DebugRenderer* debug_renderer = Lemur::Graphics::Graphics::Instance().GetDebugRenderer();
    debug_renderer->DrawCylinder(position, radius, height, { 0,1,0,1 });
}

void Enemy_D::DrawDebugGUI(int n)
{
    Character::DrawDebugGUI("Enemy_D", n);
}

void Enemy_D::UpdateAttackState(float elapsed_time)
{
    // �^�C�}�[�����Z
    attack_timer += elapsed_time;
    // �U���Ԋu�ƃ^�C�}�[���ׂ�
    if (attack_timer >= attack_interval)
    {
        // ��ɓ���������
        if (Fence::Instance().ApplyDamage(attack_power))
        {
            //TODO ����̓A�j���[�V������������v�ύX
            model->PlayAnimation(Animation::Attack, false);
            // �^�C�}�[��0��
            attack_timer = 0.0f;
        }
    }

    //TODO ����̓A�j���[�V������������v�ύX
    if (!model->IsPlayAnimation())
    {
        model->PlayAnimation(7, true);
    }

    // ���S���͎��S�X�e�[�g�֐؂�ւ�
    if (IsDead())
    {
        TransitionDeathState();
    }
}

void Enemy_D::UpdateMoveState(float elapsed_time)
{
    // ����ɓ���������
    if (Collision::IntersectRectVsCircle(Fence::Instance().left_rect, { position.x,position.z }, radius))
    {
        TransitionAttackState();
    }

    switch (move_state)
    {
    case 0:
        // ���ړ�
        velocity.x = speed_power;
        // �ړ��ʂ��L�^
        dis += abs(speed_power) * elapsed_time;
        // �ړ��ʂ��K��l�𒴂�����
        if (dis >= dis_max)
        {
            dis = 0.0f;                             // �ړ��ʂ�������
            if (is_last_touched)direction_state = 0;// �O���ɐG��ċ�����ړ��������Œ�
            else direction_state = rand() % 2;      // ����̕���������
            is_last_touched = false;                // ������
            move_state = 1;                         // �ړ���Ԃ�ω�
        }
        break;
    case 1:
        // ���ړ�
        velocity.x = speed_power;
        // �ړ��ʂ��L�^
        dis += abs(speed_power) * elapsed_time;

        // �ړ��ʂ��K��l�𒴂�����or���ɍs����������
        if (dis >= dis_max || is_last_touched)
        {
            dis = 0.0f;       // �ړ��ʂ�������
            velocity.z = 0.0f;// ���x��������
            move_state = 0;   // �ړ���Ԃ�ω�
        }

        // �c�ړ�
        switch (direction_state)
        {
        case 0:
            velocity.z = speed_power_Y;// �c�ړ�

            // �������ɐU��؂��Ă�����
            if (Collision::IntersectRectVsCircle(Fence::Instance().back_rect, { position.x,position.z }, radius))
            {
                is_last_touched = true;
            }
            break;
        case 1:
            velocity.z = -speed_power_Y;// �c�ړ�

            // �������ɐU��؂��Ă�����
            if (Collision::IntersectRectVsCircle(Fence::Instance().back_rect, { position.x,position.z }, radius))
            {
                is_last_touched = true;
            }
            break;
        }
        break;
    }

    // ���S�X�e�[�g��
    if (IsDead())
    {
        TransitionDeathState();
    }
}

