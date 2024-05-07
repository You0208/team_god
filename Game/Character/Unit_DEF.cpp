#include "Unit_DEF.h"
#include "EnemyManager.h"
#include "Game/Stage/StageManager.h"

//--------GreenPumpkin--------------------------------------------------

GreenPumpkin::GreenPumpkin()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    //LoadGltfModel(graphics.GetDevice(), ".\\resources\\Model_glb\\Unit\\unit2_RE.glb", true);
    LoadFBXModel(graphics.GetDevice(), ".\\resources\\Model\\Unit\\GreenPumpkin.fbx");

    attack_effect = new Effect(".\\resources\\Effect\\UNIT2_ATK\\UNIT2_ATK.efk");
    death_effect = new Effect(".\\resources\\Effect\\UNIT_DEATH\\UNIT_DEATH.efk");
    set_effect = new Effect(".\\resources\\Effect\\UNIT_SET\\UNIT_SET.efk");

    attack_times = 5;    // �U����
    attack_power = 1;    // �U����
    attack_interval = 0.5f; // �U���Ԋu
    attack_collision_range = 1.0f; // �U���͈�
    radius = 0.3f; // ���a
    height = 0.5f; // �f�o�b�O�p
    dec_pos = 1.0f; // ���j�b�g�ɐڐG�����킪�ǂ̂��炢���˕Ԃ���邩

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

    UpdateTransform();


    // �Ƃ肠�����A�j���[�V����
    PlayAnimation(Animation::Idle, true);
}

void GreenPumpkin::DrawDebugPrimitive()
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

void GreenPumpkin::Update(float elapsed_time)
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

void GreenPumpkin::UpdateIdleState(float elapsed_time)
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

void GreenPumpkin::UpdateAttackState(float elapsed_time)
{
    EnemyManager& enemyManager = EnemyManager::Instance();
    int enemyCount = enemyManager.GetEnemyCount();
    bool is_intersected = false;

    attack_timer += elapsed_time;// �U���^�C�}�[
    // �^�C�}�[���K�莞�Ԃ𒴂�����U��
    if (attack_timer >= attack_interval)is_attack = true;

    if (attack_times > 0)// �U���񐔂��c���Ă���Ƃ�
    {
        //TODO ���[�V������������܂��ς���
        // �G�̑�������
        for (int j = 0; j < enemyCount; ++j)
        {
            Enemy* enemy = enemyManager.GetEnemy(j);

            // ���j�b�g������ł���R���e�B�j���[
            if (enemy->IsDead())continue;

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
                if (is_attack)
                {
                    // �A�j���[�V�����̐؂�ւ�
                    PlayAnimation(Animation::Attack, false);
                    // �U���t���O��ON�Ȃ�_���[�W����
                    enemy->ApplyDamage(ReturnDamage());
                }
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
               
                if (is_attack)
                {
                    // �A�j���[�V�����̐؂�ւ�
                    PlayAnimation(Animation::Attack, false);
                    // �U���t���O��ON�Ȃ�_���[�W����
                    enemy->ApplyDamage(ReturnDamage());
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

void GreenPumpkin::DrawDebugGUI(int n)
{
    Character::DrawDebugGUI("GreenPumpkin", n);
}


//--------Broccoli--------------------------------------------------

Broccoli::Broccoli()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    // LoadGltfModel(graphics.GetDevice(), ".\\resources\\Model_glb\\Unit\\Broccoli.glb",true);
    LoadFBXModel(graphics.GetDevice(), ".\\resources\\Model\\Unit\\Broccoli.fbx");

    attack_times = 5;    // �U����
    attack_power = 1;    // �U����
    attack_width = 1.0f;    // �U����
    attack_interval = 0.5f; // �U���Ԋu
    attack_collision_range = 1.0f; // �U���͈�
    radius = 0.3f; // ���a
    height = 0.5f; // �f�o�b�O�p
    dec_pos = 1.0f; // ���j�b�g�ɐڐG�����킪�ǂ̂��炢���˕Ԃ���邩
    Rect stage_size = StageManager::Instance().GetStage(0)->GetStageCollision();
    attack_rect =
    {
        {stage_size.left_up.x,position.z + attack_width},
        {stage_size.right_down.x,position.z - attack_width},
    };

    // �Ƃ肠�����A�j���[�V����
    PlayAnimation(Animation::Idle, true);
}

void Broccoli::DrawDebugPrimitive()
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

void Broccoli::Update(float elapsed_time)
{
    Rect stage_size = StageManager::Instance().GetStage(0)->GetStageCollision();
    attack_rect =
    {
        {stage_size.left_up.x,position.z + attack_width},
        {stage_size.right_down.x,position.z - attack_width},
    };

    Unit::Update(elapsed_time);
}

void Broccoli::UpdateIdleState(float elapsed_time)
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

void Broccoli::UpdateAttackState(float elapsed_time)
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

            // �G������ł���R���e�B�j���[
            if (enemy->IsDead())continue;

            // �G�̍U���͈͂ɓ����Ă���G�S���ɍU��
            if (Collision::IntersectRectVsCircle
            (
                attack_rect,                                        // �U���͈�
                { enemy->GetPosition().x,enemy->GetPosition().z },  // �G�̈ʒu(XZ����)
                enemy->GetRadius()                                  // �G�̓����蔻��
            ))
            {
                is_intersected = true;
                // �A�j���[�V�����̐؂�ւ�
                if (is_attack)
                {
                    PlayAnimation(Animation::Attack, false);
                    enemy->ApplyDamage(ReturnDamage());
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

void Broccoli::DrawDebugGUI(int n)
{
    Character::DrawDebugGUI("Broccoli", n);
}


//--------Cauliflower--------------------------------------------------

Cauliflower::Cauliflower()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    //LoadGltfModel(graphics.GetDevice(), ".\\resources\\Model_glb\\Unit\\Cauliflower.glb",true);
    LoadFBXModel(graphics.GetDevice(), ".\\resources\\Model\\Unit\\Cauliflower.fbx");

    attack_times = 5;    // �U����
    attack_power = 1;    // �U����
    attack_width = 1.0f;    // �U����
    attack_interval = 0.5f; // �U���Ԋu
    attack_collision_range = 1.0f; // �U���͈�
    radius = 0.3f; // ���a
    height = 0.5f; // �f�o�b�O�p
    dec_pos = 1.0f; // ���j�b�g�ɐڐG�����킪�ǂ̂��炢���˕Ԃ���邩
    Rect stage_size = StageManager::Instance().GetStage(0)->GetStageCollision();
    attack_rect =
    {
        {position.x-attack_width,stage_size.left_up.y},
        {position.x + attack_width,stage_size.right_down.y},
    };

    // �Ƃ肠�����A�j���[�V����
    PlayAnimation(Animation::Idle, true);
}

void Cauliflower::DrawDebugPrimitive()
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

void Cauliflower::Update(float elapsed_time)
{
    Rect stage_size = StageManager::Instance().GetStage(0)->GetStageCollision();
    attack_rect =
    {
        {position.x - attack_width,stage_size.left_up.y},
        {position.x + attack_width,stage_size.right_down.y},
    };

    Unit::Update(elapsed_time);
}

void Cauliflower::UpdateIdleState(float elapsed_time)
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

void Cauliflower::UpdateAttackState(float elapsed_time)
{
    EnemyManager& enemyManager = EnemyManager::Instance();
    int enemyCount = enemyManager.GetEnemyCount();
    bool is_intersected = false;

    attack_timer += elapsed_time;// �U���^�C�}�[
    // �^�C�}�[���K�莞�Ԃ𒴂�����U��
    if (attack_timer >= attack_interval)is_attack = true;

    if (attack_times > 0)// �U���񐔂��c���Ă���Ƃ�
    {
        //TODO ���[�V������������܂��ς���
        // �G�̑�������
        for (int j = 0; j < enemyCount; ++j)
        {
            Enemy* enemy = enemyManager.GetEnemy(j);

            // ���j�b�g������ł���R���e�B�j���[
            if (enemy->IsDead())continue;

            // ���j�b�g�̍U���͈͂ɓ����Ă���G�S���ɍU��
            if (Collision::IntersectRectVsCircle
            (
                attack_rect,                                        // �U���͈�
                { enemy->GetPosition().x,enemy->GetPosition().z },  // �G�̈ʒu(XZ����)
                enemy->GetRadius()                                  // �G�̓����蔻��
            ))
            {
                is_intersected = true;
                // �A�j���[�V�����̐؂�ւ�
                if (is_attack)
                {
                    PlayAnimation(Animation::Attack, false);
                    enemy->ApplyDamage(ReturnDamage());
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

void Cauliflower::DrawDebugGUI(int n)
{
    Character::DrawDebugGUI("Cauliflower", n);
}
