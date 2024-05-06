#include "Enemy_ABCD.h"
#include "EnemyManager.h"
#include "UnitManager.h"

//------EnemyA-----------------------------------------------------------------

Enemy_A::Enemy_A()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Enemy\\enemy_01.fbx");

    attack_power    = 1;        // �U����
    attack_interval = 3.0f;     // �U���Ԋu

    health          = 10;       // HP
    radius          = 1.0f;     // ���a
    height          = 1.0f;     // �f�o�b�O�p
    speed_power     = -1.0f;    // ���x

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
    if (shaft == Shaft::Side) rotation.y = DirectX::XMConvertToRadians(-90);
    else if (shaft == Shaft::Vertical)rotation.y = DirectX::XMConvertToRadians(180);

    // ��ɐG���΍U���X�e�[�g��
    if (Collision::IntersectRectVsCircle(Fence::Instance().GetLeftRect(), { position.x,position.z }, radius)||
        Collision::IntersectRectVsCircle(Fence::Instance().GetFrontRect(), { position.x,position.z }, radius))
    {
        TransitionAttackState();
    }
    else
    {
        // �ړ�
        if (shaft == Shaft::Side) velocity.x = speed_power;
        else if(shaft==Shaft::Vertical)velocity.z = speed_power;
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
    model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Enemy\\enemy_02.fbx");

    attack_power    = 1;        // �U����
    attack_interval = 3.0f;     // �U���Ԋu

    radius          = 1.0f;     // ���a
    height          = 1.0f;     // �f�o�b�O�p

    dis             = 3.0f;     // ��C�ɐi�ދ���
    move_timer      = 0.0f;     // �^�C�}�[
    move_timer_max  = 3.0f;     // �����܂ł̎���
    speed_power     = -3.0f;    // �X�s�[�h

    health          = 10;       // HP

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
    if (shaft == Shaft::Side) rotation.y = DirectX::XMConvertToRadians(-90);
    else if (shaft == Shaft::Vertical)rotation.y = DirectX::XMConvertToRadians(180);
    // ��ɓ���������
    if (Collision::IntersectRectVsCircle(Fence::Instance().GetLeftRect(), { position.x,position.z }, radius) ||
        Collision::IntersectRectVsCircle(Fence::Instance().GetFrontRect(), { position.x,position.z }, radius))
    {
        TransitionAttackState();
    }

    // �^�C�}�[�����Z
    move_timer += elapsed_time;

    //�^�C�}�[���w��b�𒴂����Ƃ�
    if (move_timer >= move_timer_max)                   
    {
        if (shaft == Shaft::Side)
        {
            // ���x����
            velocity.x = speed_power;
            // ����̋����i�񂾂�
            if (abs(old_position - position.x) >= dis)
            {
                // �Ăёҋ@
                move_timer = 0.0f;
            }
        }
        else
        {
            // ���x����
            velocity.z = speed_power;
            // ����̋����i�񂾂�
            if (abs(old_position - position.z) >= dis)
            {
                // �Ăёҋ@
                move_timer = 0.0f;
            }
        }
    }
    else
    {
        if (shaft == Shaft::Side)
        {
            // �O�ʒu��ύX
            old_position = position.x;
            // �ړ�
            velocity.x = 0.0f;
        }
        else
        {
            // �O�ʒu��ύX
            old_position = position.z;
            // �ړ�
            velocity.z = 0.0f;
        }
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
    model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Enemy\\enemy_03.fbx");

    attack_power    = 1;         // �U���́@
    attack_interval = 3.0f;      // �U���Ԋu

    radius          = 1.0f;      // ���a
    height          = 1.0f;      // �f�o�b�O�v
    //position.x      = 5.0f;      // �����ʒu
    //rotation.y      = -90.0f;    // �����p�x
    speed_power     = -1.0f;     // ���x
    health          = 10;        // HP

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
    if (shaft == Shaft::Side)
    {
        // �ړ�
        HandleMovementState(Fence::Instance().GetLeftRect(), Fence::Instance().GetFrontRect(),
            speed_power, Move::Straight, Move::Avoid, velocity.x, is_touched_unit, elapsed_time);
        //
        JudgeUnit(true);
    }
    else
    {
        // �ړ�
        HandleMovementState(Fence::Instance().GetLeftRect(), Fence::Instance().GetFrontRect(),
            speed_power, Move::Straight, Move::Avoid, velocity.z, is_touched_unit, elapsed_time);
        //
        JudgeUnit(false);
    }

    if (IsDead())
    {
        TransitionDeathState();
    }
}

void Enemy_C::JudgeUnit(bool isVertical)
{
    UnitManager& unitManager = UnitManager::Instance();
    int unitCount = unitManager.GetUnitCount();

    // ���j�b�g�ɂ̑�������
    for (int j = 0; j < unitCount; ++j)
    {
        Unit* unit = unitManager.GetUnit(j);

        // �~
        if (unit->GetCategory() == 0 || unit->GetCategory() == 3) 
        {
            if (Collision::IntersectCircleVsCircle(
                { unit->GetPosition().x, unit->GetPosition().z }, // ���j�b�g�̈ʒu(XZ����)
                unit->GetAttackCollisionRange(),                   // �U���͈�
                { position.x, position.z },                        // �G�̈ʒu(XZ����)
                radius))                                           // �G�̓����蔻��
            {
                is_touched_unit = true; // ���j�b�g�ɐG�ꂽ
                if (isVertical)
                {
                    destination = {
                        unit->GetPosition().x,
                        unit->GetPosition().z + unit->GetAttackCollisionRange() + radius
                    };
                }
                else
                {
                    destination = {
                        // ���j�b�g���ɔ�����
                        unit->GetPosition().x - radius - unit->GetAttackCollisionRange() ,
                        unit->GetPosition().z
                    };
                }
            }
        }
        else if (unit->GetCategory() == 1) // �O�p�`
        {
            if (isVertical)
            {
                // �G�����j�b�g�̍U���͈͂ɐG�ꂽ�Ƃ�
                // �E�O�p
                if (Collision::IntersectTriangleVsCircle
                (
                    unit->GetTriangle2(),
                    { position.x,position.z },                         // �G�̈ʒu(XZ����)
                    radius                                             // �G�̓����蔻��
                ))
                {
                    is_touched_unit = true;// ���j�b�g�ɐG�ꂽ
                    // �ړI�n�����肷��
                    destination = {
                        unit->GetPosition().x,
                        unit->GetPosition().z + unit->GetRadius() + radius
                    };
                }
            }
            else if (!isVertical )
            {
                // �G�����j�b�g�̍U���͈͂ɐG�ꂽ�Ƃ�
                if (Collision::IntersectTriangleVsCircle// ���O�p
                (
                    unit->GetTriangle1(),
                    { position.x,position.z },                         // �G�̈ʒu(XZ����)
                    radius                                             // �G�̓����蔻��
                ))
                {
                    is_touched_unit = true;// ���j�b�g�ɐG�ꂽ
                    // �ړI�n�����肷��
                    destination = {
                        // ���j�b�g���O�p�`�̒�ӕ����ɔ�����
                        unit->GetPosition().x - unit->GetRadius() - radius - unit->GetTriangleHeight(),
                        unit->GetPosition().z
                    };
                }
                // �G�����j�b�g�̍U���͈͂ɐG�ꂽ�Ƃ�
                else if (Collision::IntersectTriangleVsCircle// �E�O�p
                (
                    unit->GetTriangle2(),
                    { position.x,position.z },                         // �G�̈ʒu(XZ����)
                    radius                                             // �G�̓����蔻��
                ))
                {
                    is_touched_unit = true;// ���j�b�g�ɐG�ꂽ
                    // �ړI�n�����肷��
                    destination = {
                        // ���j�b�g�E�O�p�`�̒�ӕ����ɔ�����
                        unit->GetPosition().x + unit->GetRadius() + radius + unit->GetTriangleHeight(),
                        unit->GetPosition().z
                    };
                }
            }
        }
        else if (unit->GetCategory() == 2)
        {
            if (isVertical)
            {
                // �G�����j�b�g�̍U���͈͂ɐG�ꂽ�Ƃ�
          // ���O�p
                if (Collision::IntersectTriangleVsCircle
                (
                    unit->GetTriangle1(),
                    { position.x,position.z },                         // �G�̈ʒu(XZ����)
                    radius                                             // �G�̓����蔻��
                ))
                {
                    is_touched_unit = true;// ���j�b�g�ɐG�ꂽ
                    // �ړI�n�����肷��
                    destination = {
                        unit->GetPosition().x,
                        unit->GetPosition().z + unit->GetRadius() + radius + unit->GetTriangleHeight()
                    };
                }
                // �G�����j�b�g�̍U���͈͂ɐG�ꂽ�Ƃ�
                // ��O�O�p
                else if (Collision::IntersectTriangleVsCircle
                (
                    unit->GetTriangle2(),
                    { position.x,position.z },                         // �G�̈ʒu(XZ����)
                    radius                                             // �G�̓����蔻��
                ))
                {
                    is_touched_unit = true;// ���j�b�g�ɐG�ꂽ
                    // �ړI�n�����肷��
                    destination = {
                        unit->GetPosition().x,
                        unit->GetPosition().z - unit->GetRadius() - radius - unit->GetTriangleHeight()
                    };
                }
            }
            else if (!isVertical)
            {
                // �G�����j�b�g�̍U���͈͂ɐG�ꂽ�Ƃ�
                if (Collision::IntersectTriangleVsCircle// ���O�p
                (
                    unit->GetTriangle1(),
                    { position.x,position.z },                         // �G�̈ʒu(XZ����)
                    radius                                             // �G�̓����蔻��
                ))
                {
                    is_touched_unit = true;// ���j�b�g�ɐG�ꂽ
                    // �ړI�n�����肷��
                    destination = {
                        // ���j�b�g���ɔ�����
                        unit->GetPosition().x - unit->GetRadius() - radius,
                        unit->GetPosition().z
                    };
                }
            }
        }
        else if (unit->GetCategory() == 4 && isVertical) // �����`
        {
            // �G�����j�b�g�̍U���͈͂ɐG�ꂽ�Ƃ�
            if (Collision::IntersectRectVsCircle
            (
                unit->GetAttackRect(),
                { position.x,position.z },                         // �G�̈ʒu(XZ����)
                radius                                             // �G�̓����蔻��
            ))
            {
                is_touched_unit = true;// ���j�b�g�ɐG�ꂽ
                // �ړI�n�����肷��
                destination = {
                    unit->GetPosition().x,
                    unit->GetPosition().z + unit->GetRadius() + radius
                };
            }
        }
        else if (unit->GetCategory() == 5 && !isVertical)
        {
            // �G�����j�b�g�̍U���͈͂ɐG�ꂽ�Ƃ�
            if (Collision::IntersectRectVsCircle
            (
                unit->GetAttackRect(),
                { position.x,position.z },                         // �G�̈ʒu(XZ����)
                radius                                             // �G�̓����蔻��
            ))
            {
                is_touched_unit = true;// ���j�b�g�ɐG�ꂽ
                // �ړI�n�����肷��
                destination = {
                    unit->GetPosition().x - unit->GetRadius() - radius,
                    unit->GetPosition().z
                };
            }
        }
    }
}

void Enemy_C::HandleMovementState(const Rect& rect1, const Rect& rect2, float speedPower, Move straightState, Move avoidState, float& velocityComponent, bool& touchedUnit, float elapsed_time)
{
    // ��ɐG�ꂽ��
    if (CheckFenceCollision(rect1, rect2))
    {
        // �U���X�e�[�g��
        TransitionAttackState();
    }
    else
    {
        switch (move_state)
        {
        case Move::Straight:
            // �܂�����������
            velocityComponent = speedPower;
            // ���j�b�g�ɐG�ꂽ��X�e�[�g��ύX
            if (touchedUnit) move_state = avoidState;
            break;

        case Move::Avoid:
            // �G��ړI�n�܂œ�����
            MoveToDestination(elapsed_time, straightState);
            break;
        }
    }
}

void Enemy_C::MoveToDestination(float elapsed_time, Move nextState)
{
    DirectX::XMFLOAT2 destVec = { destination.x - position.x, destination.y - position.z };
    destVec = Normalize(destVec);

    destVec = { destVec.x * abs(speed_power), destVec.y * abs(speed_power) };

    velocity.x = destVec.x;
    velocity.z = destVec.y;

    if (ReachedDestination())
    {
        ResetMovementState(nextState);// �ړ���Ԃ̃��Z�b�g
    }
}

//-------EnemyD---------------------------------------------------------------

Enemy_D::Enemy_D()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Enemy\\enemy_04.fbx");

    attack_power    = 1;        // �U����
    attack_interval = 3.0f;     // �U���Ԋu

    radius          = 1.0f;     // ���a
    height          = 1.0f;     // �f�o�b�O�p

    move_state      = 0;        // �ړ����
    speed_power     = -1.0f;    // ���ړ���
    speed_power_Y   = -1.0f;    // �c�ړ���
    direction_state = 0;        // �����̏��
    dis_max         = 1.0f;     // �ړ��ʋK��l
    is_last_touched = false;    // �O���ɐG�ꂽ��

    health          = 10;       // HP

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

void Enemy_D::UpdateMoveState_S(float elapsed_time)
{
    switch (move_state)
    {
    case Move::Straight:
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
    case Move::Diagonal:
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
        case Direction::Under:
            velocity.z = speed_power_Y;// �c�ړ�

            // �������ɐU��؂��Ă�����
            if (Collision::IntersectRectVsCircle(Fence::Instance().GetBackRect(), { position.x,position.z }, radius))
            {
                is_last_touched = true;
            }
            break;
        case Direction::Up:
            velocity.z = -speed_power_Y;// �c�ړ�

            // �������ɐU��؂��Ă�����
            if (Collision::IntersectRectVsCircle(Fence::Instance().GetBackRect(), { position.x,position.z }, radius))
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

void Enemy_D::UpdateMoveState_V(float elapsed_time)
{
    switch (move_state)
    {
    case Move::Straight:
        // �c�ړ�
        velocity.z = speed_power;
        // �ړ��ʂ��L�^
        dis += abs(speed_power) * elapsed_time;
        // �ړ��ʂ��K��l�𒴂�����
        if (dis >= dis_max)
        {
            dis = 0.0f;                             // �ړ��ʂ�������
            if (is_last_touched)direction_state = 0;// �O���ɐG��ċ�����ړ��������Œ�
            else direction_state = rand() % 2;      // ����̕���������
            is_last_touched = false;                // ������
            move_state = Move::Diagonal;            // �ړ���Ԃ�ω�
        }
        break;
    case Move::Diagonal:
        // �c�ړ�
        velocity.z = speed_power;
        // �ړ��ʂ��L�^
        dis += abs(speed_power) * elapsed_time;

        // �ړ��ʂ��K��l�𒴂�����or�E�ɍs����������
        if (dis >= dis_max || is_last_touched)
        {
            dis = 0.0f;                     // �ړ��ʂ�������
            velocity.x = 0.0f;              // ���x��������
            move_state = Move::Straight;    // �ړ���Ԃ�ω�
        }

        // �c�ړ�
        switch (direction_state)
        {
        case Direction::Under:// �E��
            velocity.x = speed_power_Y;// �E�΂߈ړ�

            // �������ɐU��؂��Ă�����
            if (Collision::IntersectRectVsCircle(Fence::Instance().GetRightRect(), { position.x,position.z }, radius))
            {
                is_last_touched = true;
            }
            break;
        case Direction::Up:// ����
            velocity.x = -speed_power_Y;// �c�ړ�

            // �������ɐU��؂��Ă�����
            if (Collision::IntersectRectVsCircle(Fence::Instance().GetRightRect(), { position.x,position.z }, radius))
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

void Enemy_D::UpdateMoveState(float elapsed_time)
{
    // ��ɓ���������
    if (Collision::IntersectRectVsCircle(Fence::Instance().GetLeftRect(), { position.x,position.z }, radius)||
        Collision::IntersectRectVsCircle(Fence::Instance().GetFrontRect(), { position.x,position.z }, radius))
    {
        TransitionAttackState();
    }

    if (shaft == Shaft::Side)UpdateMoveState_S(elapsed_time);
    else    UpdateMoveState_V(elapsed_time);
 
}

