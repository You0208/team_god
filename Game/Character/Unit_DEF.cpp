#include "Unit_DEF.h"
#include "UnitManager.h"
#include "EnemyManager.h"
#include "Game/Stage/StageManager.h"

//--------Unit_D--------------------------------------------------

Unit_D::Unit_D()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Unit\\unit4_RE.fbx");

    attack_times            = 5;    // �U����
    attack_power            = 1;    // �U����
    streng_width            = 2;    // ������
    attack_interval         = 0.5f; // �U���Ԋu
    attack_collision_range  = 1.0f; // �U���͈�
    radius                  = 0.4f; // ���a
    height                  = 0.5f; // �f�o�b�O�p
    dec_pos                 = 1.0f; // ���j�b�g�ɐڐG�����킪�ǂ̂��炢���˕Ԃ���邩
    timer_max               = 1.0f; // �o�t�̕b��

    // �Ƃ肠�����A�j���[�V����
    model->PlayAnimation(Animation::Idle, true);
}

void Unit_D::DrawDebugPrimitive()
{
    DebugRenderer* debug_renderer = Lemur::Graphics::Graphics::Instance().GetDebugRenderer();
    debug_renderer->DrawCylinder(position, attack_collision_range, height, { 1,0,0,1 });
    debug_renderer->DrawCylinder(position, radius, height, { 0,1,0,1 });
}

void Unit_D::UpdateIdleState(float elapsed_time)
{
    UnitManager& unitManager = UnitManager::Instance();
    int unitCount = unitManager.GetUnitCount();

    attack_timer += elapsed_time;// �U���^�C�}�[

    //TODO ���[�V������������܂��ς���
    // ���j�b�g�̑�������
    for (int j = 0; j < unitCount; ++j)
    {
        Unit* unit = unitManager.GetUnit(j);

        // ���̃��j�b�g�����j�b�g�̍U���͈͂ɓ����Ă���Ƃ�
        if (Collision::IntersectCircleVsCircle
        (
            { position.x,position.z },                        // ���j�b�g�̈ʒu(XZ����)
            attack_collision_range,                           // �U���͈�
            { unit->GetPosition().x,unit->GetPosition().z },  // �G�̈ʒu(XZ����)
            unit->GetRadius()                                 // �G�̓����蔻��
        ))
        {
            // ������Ԃ�true��
            unit->SetBuff(true);
            unit->SetStrengAttack(unit->GetAttackPower() + streng_width);
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

void Unit_D::UpdateAttackState(float elapsed_time)
{
    attack_timer += elapsed_time;// �U���^�C�}�[

    UnitManager& unitManager = UnitManager::Instance();
    int unitCount = unitManager.GetUnitCount();
    bool is_intersected = false;

    //TODO ���[�V������������܂��ς���
    // ���j�b�g�̑�������
    for (int j = 0; j < unitCount; ++j)
    {
        Unit* unit = unitManager.GetUnit(j);

        // ���̃��j�b�g�����j�b�g�̍U���͈͂ɓ����Ă���Ƃ�
        if (Collision::IntersectCircleVsCircle
        (
            { position.x,position.z },                        // ���j�b�g�̈ʒu(XZ����)
            attack_collision_range,                           // �U���͈�
            { unit->GetPosition().x,unit->GetPosition().z },  // �G�̈ʒu(XZ����)
            unit->GetRadius()                                 // �G�̓����蔻��
        ))
        {
            is_intersected = true;
            // ������Ԃ�true��
            unit->SetBuff(true);
            unit->SetStrengAttack(unit->GetAttackPower() + streng_width);
        }
        else// �͈͓��ɂ��Ȃ��G��
        {
            // ������Ԃ�؂�
            unit->SetBuff(true);
        }
    }

    // �͈͓��ɓG����̂����Ȃ���Αҋ@
    if (!is_intersected)
    {
        TransitionIdleState();
    }

    // �U�����Ԃ��߂��������
    if (attack_timer >= timer_max)
    {
        TransitionDeathState();
    }
}

void Unit_D::DrawDebugGUI(int n)
{
    Character::DrawDebugGUI("Unit_D", n);
}


//--------Unit_E--------------------------------------------------

Unit_E::Unit_E()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Unit\\unit5_RE.fbx");

    attack_times = 5;    // �U����
    attack_power = 1;    // �U����
    attack_width = 1.0f;    // �U����
    attack_interval = 0.5f; // �U���Ԋu
    attack_collision_range = 1.0f; // �U���͈�
    radius = 0.4f; // ���a
    height = 0.5f; // �f�o�b�O�p
    dec_pos = 1.0f; // ���j�b�g�ɐڐG�����킪�ǂ̂��炢���˕Ԃ���邩
    Rect stage_size = StageManager::Instance().GetStage(0)->GetStageCollision();
    attack_rect =
    {
        {stage_size.left_up.x,position.z + attack_width},
        {stage_size.right_down.x,position.z - attack_width},
    };

    // �Ƃ肠�����A�j���[�V����
    model->PlayAnimation(Animation::Idle, true);
}

void Unit_E::DrawDebugPrimitive()
{
    DebugRenderer* debug_renderer = Lemur::Graphics::Graphics::Instance().GetDebugRenderer();
    DirectX::XMFLOAT3 attack_rect_size = {
        (attack_rect.right_down.x - attack_rect.left_up.x) * 0.5f,
        0.5f,
        (attack_rect.left_up.y - attack_rect.right_down.y) * 0.5f
    };
    DirectX::XMFLOAT3 attack_rect_center = {
        attack_rect.left_up.x + attack_rect_size.x,
        0,
        attack_rect.right_down.y + attack_rect_size.z
    };

    debug_renderer->DrawBox(attack_rect_center, { 0,0,0 }, attack_rect_size, { 1,0,1,1 });
}

void Unit_E::Update(float elapsed_time)
{
    Rect stage_size = StageManager::Instance().GetStage(0)->GetStageCollision();
    attack_rect =
    {
        {stage_size.left_up.x,position.z + attack_width},
        {stage_size.right_down.x,position.z - attack_width},
    };

    Unit::Update(elapsed_time);
}

void Unit_E::UpdateIdleState(float elapsed_time)
{
    EnemyManager& enemyManager = EnemyManager::Instance();
    int enemyCount = enemyManager.GetEnemyCount();

    //TODO ���[�V������������܂��ς���
    // �G�̑�������
    for (int j = 0; j < enemyCount; ++j)
    {
        Enemy* enemy = enemyManager.GetEnemy(j);

        // �G�����j�b�g�̍U���͈͂ɓ����Ă���Ƃ�
        if (Collision::IntersectRectVsCircle
        (
            attack_rect,
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

void Unit_E::UpdateAttackState(float elapsed_time)
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
        if (Collision::IntersectRectVsCircle
        (
            attack_rect,                                        // �U���͈�
            { enemy->GetPosition().x,enemy->GetPosition().z },  // �G�̈ʒu(XZ����)
            enemy->GetRadius()                                  // �G�̓����蔻��
        ))
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

void Unit_E::DrawDebugGUI(int n)
{
    Character::DrawDebugGUI("Unit_E", n);
}


//--------Unit_F--------------------------------------------------

Unit_F::Unit_F()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Unit\\unit6_RE.fbx");

    attack_times = 5;    // �U����
    attack_power = 1;    // �U����
    attack_width = 1.0f;    // �U����
    attack_interval = 0.5f; // �U���Ԋu
    attack_collision_range = 1.0f; // �U���͈�
    radius = 0.4f; // ���a
    height = 0.5f; // �f�o�b�O�p
    dec_pos = 1.0f; // ���j�b�g�ɐڐG�����킪�ǂ̂��炢���˕Ԃ���邩
    Rect stage_size = StageManager::Instance().GetStage(0)->GetStageCollision();
    attack_rect =
    {
        {position.x-attack_width,stage_size.left_up.y},
        {position.x + attack_width,stage_size.right_down.y},
    };

    // �Ƃ肠�����A�j���[�V����
    model->PlayAnimation(Animation::Idle, true);
}

void Unit_F::DrawDebugPrimitive()
{
    DebugRenderer* debug_renderer = Lemur::Graphics::Graphics::Instance().GetDebugRenderer();
    DirectX::XMFLOAT3 attack_rect_size = {
        (attack_rect.right_down.x - attack_rect.left_up.x) * 0.5f,
        0.5f,
        (attack_rect.left_up.y - attack_rect.right_down.y) * 0.5f
    };
    DirectX::XMFLOAT3 attack_rect_center = {
        attack_rect.left_up.x + attack_rect_size.x,
        0,
        attack_rect.right_down.y + attack_rect_size.z
    };

    debug_renderer->DrawBox(attack_rect_center, { 0,0,0 }, attack_rect_size, { 1,0,1,1 });
}

void Unit_F::Update(float elapsed_time)
{
    Rect stage_size = StageManager::Instance().GetStage(0)->GetStageCollision();
    attack_rect =
    {
        {position.x - attack_width,stage_size.left_up.y},
        {position.x + attack_width,stage_size.right_down.y},
    };

    Unit::Update(elapsed_time);
}

void Unit_F::UpdateIdleState(float elapsed_time)
{
    EnemyManager& enemyManager = EnemyManager::Instance();
    int enemyCount = enemyManager.GetEnemyCount();

    //TODO ���[�V������������܂��ς���
    // �G�̑�������
    for (int j = 0; j < enemyCount; ++j)
    {
        Enemy* enemy = enemyManager.GetEnemy(j);

        // �G�����j�b�g�̍U���͈͂ɓ����Ă���Ƃ�
        if (Collision::IntersectRectVsCircle
        (
            attack_rect,
            { enemy->GetPosition().x,enemy->GetPosition().z },  // �G�̈ʒu(XZ����)
            enemy->GetRadius()                                  // �G�̓����蔻��
        ))
        {
            // �U���X�e�[�g�ɐ؂�ւ�
            TransitionAttackState();
        }
    }
    // �U���񐔂���������������
    if (attack_times <= 0 || death)
    {
        TransitionDeathState();
    }
}

void Unit_F::UpdateAttackState(float elapsed_time)
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
        if (Collision::IntersectRectVsCircle
        (
            attack_rect,                                        // �U���͈�
            { enemy->GetPosition().x,enemy->GetPosition().z },  // �G�̈ʒu(XZ����)
            enemy->GetRadius()                                  // �G�̓����蔻��
        ))
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

void Unit_F::DrawDebugGUI(int n)
{
    Character::DrawDebugGUI("Unit_F", n);
}
