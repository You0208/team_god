#include "Unit_ABC.h"
#include "EnemyManager.h"
#include "UnitManager.h"

//--------Chili--------------------------------------------------
Chili::Chili()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    //LoadGltfModel(graphics.GetDevice(), ".\\resources\\Model_glb\\Unit\\Chili.glb",true);
    LoadFBXModel(graphics.GetDevice(), ".\\resources\\Model\\Unit\\Chili.fbx");

    collision_model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Collision\\circle.fbx");


    attack_effect = new Effect(".\\resources\\Effect\\UNIT1\\UNIT1.efk");
    death_effect = new Effect(".\\resources\\Effect\\UNIT_DEATH\\UNIT_DEATH.efk");
    set_effect = new Effect(".\\resources\\Effect\\UNIT_SET\\UNIT_SET.efk");

    attack_times           = 5;    // �U����
    attack_power           = 1;    // �U����
    attack_interval        = 0.1f; // �U���Ԋu
    attack_collision_range = 1.0f; // �U���͈�
    height                 = 0.5f; // �f�o�b�O�p
    dec_pos                = 1.0f; // ���j�b�g�ɐڐG�����킪�ǂ̂��炢���˕Ԃ���邩

    radius                 = 0.4f; // ���a

    attack_effect_size = 0.5f;
    set_effect_size = 0.5f;
    death_effect_size = 0.5f;

    UpdateTransform();

    // �Ƃ肠�����A�j���[�V����
    PlayAnimation(Animation::Idle, true);
}

void Chili::DrawDebugPrimitive()
{
    //DebugRenderer* debug_renderer = Lemur::Graphics::Graphics::Instance().GetDebugRenderer();
    //debug_renderer->DrawCylinder(position, attack_collision_range, height, { 1,0,0,1 });
    //debug_renderer->DrawCylinder(position, radius, height, { 0,1,0,1 });
}

void Chili::UpdateIdleState(float elapsed_time)
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

void Chili::UpdateAttackState(float elapsed_time)
{
    EnemyManager& enemyManager = EnemyManager::Instance();
    int enemyCount = enemyManager.GetEnemyCount();

    // �G���͈͓��ɂ��邩�ǂ���
    bool is_intersected = false;

    attack_timer += elapsed_time;// �U���Ԋu�p�^�C�}�[

    // �Q���ڈȍ~�̓^�C�}�[���K�莞�Ԃ𒴂�����U��
    if (attack_timer >= attack_interval)is_attack = true;

    if (attack_times > 0)// �U���񐔂��c���Ă���Ƃ�
    {
        // �G�̑�������
        for (int j = 0; j < enemyCount; ++j)
        {
            Enemy* enemy = enemyManager.GetEnemy(j);

            // �G������ł���R���e�B�j���[
            if (enemy->IsDead())continue;

            // ���j�b�g�̍U���͈͂ɓ����Ă���G�S���ɍU��
            if (Collision::IntersectCircleVsCircle
            (
                { position.x,position.z },                          // ���j�b�g�̈ʒu(XZ����)
                attack_collision_range,                             // �U���͈�
                { enemy->GetPosition().x,enemy->GetPosition().z },  // �G�̈ʒu(XZ����)
                enemy->GetRadius()                                  // �G�̓����蔻��
            ))
            {
                // �G�Ƃ��Ԃ����t���O��ON
                is_intersected = true;
                if (is_attack)
                {
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
        if(!IsPlayAnimation()) TransitionDeathState();
    }

}

void Chili::DrawDebugGUI(int n)
{
    Character::DrawDebugGUI("Chili", n);
}


//--------Shishito--------------------------------------------------

Shishito::Shishito()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    //LoadGltfModel(graphics.GetDevice(), ".\\resources\\Model_glb\\Unit\\Shishito.glb",true);
    LoadFBXModel(graphics.GetDevice(), ".\\resources\\Model\\Unit\\Shishito.fbx");

    collision_model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Collision\\circle.fbx");
    collision_scale_facter = 2.0f;

    attack_effect = new Effect(".\\resources\\Effect\\UNIT4_BUFF\\UNIT4_BUFF.efk");
    death_effect = new Effect(".\\resources\\Effect\\UNIT_DEATH\\UNIT_DEATH.efk");
    set_effect = new Effect(".\\resources\\Effect\\UNIT_SET\\UNIT_SET.efk");

    attack_times = 5;    // �U����
    attack_power = 1;    // �U����
    streng_width = 2;    // ������
    attack_interval = 0.5f; // �U���Ԋu
    attack_collision_range = 1.0f; // �U���͈�
    height = 0.5f; // �f�o�b�O�p
    dec_pos = 1.0f; // ���j�b�g�ɐڐG�����킪�ǂ̂��炢���˕Ԃ���邩
    timer_max = 3.0f; // �o�t�̕b��
    radius = 0.4f; // ���a

    // �Ƃ肠�����A�j���[�V����
    PlayAnimation(Animation::Idle, true);
}

void Shishito::DrawDebugPrimitive()
{
   //DebugRenderer* debug_renderer = Lemur::Graphics::Graphics::Instance().GetDebugRenderer();
   //debug_renderer->DrawCylinder(position, attack_collision_range, height, { 1,0,0,1 });
   //debug_renderer->DrawCylinder(position, radius, height, { 0,1,0,1 });
}

void Shishito::UpdateIdleState(float elapsed_time)
{
    UnitManager& unitManager = UnitManager::Instance();
    int unitCount = unitManager.GetUnitCount();

    // �U���X�e�[�g�ɐ؂�ւ�
    PlayAnimation(Animation::Attack, true);
    TransitionAttackState();

    attack_timer += elapsed_time;// �U���^�C�}�[

    // �����I�ɃA�^�b�N��
    TransitionAttackState();

    //TODO ���[�V������������܂��ς���
    // ���j�b�g�̑�������
    for (int j = 0; j < unitCount; ++j)
    {
        Unit* unit = unitManager.GetUnit(j);
        if (unit->GetCategory() == UnitManager::UNIT_INDEX::Shishito)continue;
        // ���j�b�g������ł���R���e�B�j���[
        if (unit->IsDead())continue;

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

            // �A�j���[�V�����ƃG�t�F�N�g
            attack_handle = attack_effect->Play(position, attack_effect_size);

            // �U���X�e�[�g�ɐ؂�ւ�
            PlayAnimation(Animation::Attack, false);
            TransitionAttackState();
        }
    }
    // �U���񐔂���������������
    if (attack_timer >= timer_max)
    {
        TransitionDeathState();
    }
}

void Shishito::UpdateAttackState(float elapsed_time)
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

        if (unit->GetCategory() == UnitManager::UNIT_INDEX::Shishito)continue;
        // ���j�b�g������ł���R���e�B�j���[
        if (unit->IsDead())continue;

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
            if (!IsPlayAnimation())
            {
                PlayAnimation(Animation::Attack, false);
                attack_handle = attack_effect->Play(position, attack_effect_size);
            }
        }
        else// �͈͓��ɂ��Ȃ�
        {
            // ������Ԃ�؂�
            unit->SetBuff(true);
        }
    }

    //// �͈͓��ɓG����̂����Ȃ���Αҋ@
    //if (!is_intersected)
    //{
    //    TransitionIdleState();
    //}

    // �U�����Ԃ��߂��������
    if (attack_timer >= timer_max)
    {
        attack_effect->Stop(attack_handle);
        TransitionDeathState();
    }
}

void Shishito::DrawDebugGUI(int n)
{
    Character::DrawDebugGUI("Shishito", n);
}


//--------OrangePumpkin--------------------------------------------------

OrangePumpkin::OrangePumpkin()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    //LoadGltfModel(graphics.GetDevice(), ".\\resources\\Model_glb\\Unit\\OrangePumpkin.glb",true);
    LoadFBXModel(graphics.GetDevice(), ".\\resources\\Model\\Unit\\OrangePumpkin.fbx");

    collision_model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Collision\\triangle_collision.fbx");

    attack_effect = new Effect(".\\resources\\Effect\\UNIT2_ATK\\UNIT2_ATK.efk");
    death_effect = new Effect(".\\resources\\Effect\\UNIT_DEATH\\UNIT_DEATH.efk");
    set_effect = new Effect(".\\resources\\Effect\\UNIT_SET\\UNIT_SET.efk");

    attack_times = 5;    // �U����
    attack_power = 1;    // �U����
    attack_interval = 0.5f; // �U���Ԋu
    attack_collision_range = 1.0f; // �U���͈�
    radius = 0.4f; // ���a
    height = 0.5f; // �f�o�b�O�p
    dec_pos = 1.0f; // ���j�b�g�ɐڐG�����킪�ǂ̂��炢���˕Ԃ���邩

    // �O�p
    t_height               = 1.0f;// ����
    t_base                 = 1.0f;// ���

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

void OrangePumpkin::DrawDebugPrimitive()
{
    //DebugRenderer* debug_renderer = Lemur::Graphics::Graphics::Instance().GetDebugRenderer();
    //debug_renderer->DrawCylinder(position, radius, height, { 0,1,0,1 });

    //// ���O�p
    //debug_renderer->DrawSphere({ triangle_1.A.x,0.2f,triangle_1.A.y }, 0.1f, { 1,0,1,1 });
    //debug_renderer->DrawSphere({ triangle_1.B.x,0.2f,triangle_1.B.y }, 0.1f, { 1,0,1,1 });
    //debug_renderer->DrawSphere({ triangle_1.C.x,0.2f,triangle_1.C.y }, 0.1f, { 1,0,1,1 });

    //// ��O�O�p
    //debug_renderer->DrawSphere({ triangle_2.A.x,0.2f,triangle_2.A.y }, 0.1f, { 0,0,1,1 });
    //debug_renderer->DrawSphere({ triangle_2.B.x,0.2f,triangle_2.B.y }, 0.1f, { 0,0,1,1 });
    //debug_renderer->DrawSphere({ triangle_2.C.x,0.2f,triangle_2.C.y }, 0.1f, { 0,0,1,1 });
}

void OrangePumpkin::Update(float elapsed_time)
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

void OrangePumpkin::UpdateIdleState(float elapsed_time)
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

void OrangePumpkin::UpdateAttackState(float elapsed_time)
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
                    // �G�t�F�N�g�̍Đ�
                    attack_handle = attack_effect->Play(position, attack_effect_size);
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
void OrangePumpkin::DrawDebugGUI(int n)
{
    Character::DrawDebugGUI("OrangePumpkin", n);
}
