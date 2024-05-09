#include "UniqueEnemy.h"
#include "Enemy_ABCD.h"
#include "EnemyManager.h"
#include "Lemur/Input/Input.h"

SummonEnemy::SummonEnemy(bool is_minor)
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    if (!is_minor)LoadFBXModel(graphics.GetDevice(), ".\\resources\\Model\\Enemy\\Bear.fbx");
    else LoadFBXModel(graphics.GetDevice(), ".\\resources\\Model\\Enemy\\Bear2.fbx");

    death_effect = new Effect(".\\resources\\Effect\\ENEMY_DOWN\\ENEMY_DOWN.efk");
    hit_effect = new Effect(".\\resources\\Effect\\hit_to_enemy\\hit_to_enemy.efk");

    attack_power = 1;        // �U����
    attack_interval = 3.0f;     // �U���Ԋu

    health = 10;       // HP
    radius = 1.0f;     // ���a
    height = 1.0f;     // �f�o�b�O�p
    speed_power = -1.0f;    // ���x

    
    // TODO ����
    // �o���G�̃X�e�[�^�X�X�^�b�N
    status[0].attack_power = 1;              //�U����
    status[0].attack_interval = 0.5f;        //�U���Ԋu
    status[0].health = 3;                    // HP
    status[0].speed_power = -0.5f;            // ���x
    status[0].radius = 0.5f;                 // ���a
    status[0].attack_effect_size = 0.5f;     // �U���G�t�F�N�g�̃T�C�Y
    status[0].death_effect_size = 0.5f;      // ���S�G�t�F�N�g�̃T�C�Y
    status[0].hit_effect_size = 0.5f;        // �ݒu�G�t�F�N�g�̃T�C�Y

    status[1].attack_power = 1;              //�U����
    status[1].attack_interval = 0.5f;        //�U���Ԋu
    status[1].health = 3;                    // HP
    status[1].speed_power = -0.5f;            // ���x
    status[1].radius = 0.5f;                 // ���a
    status[1].attack_effect_size = 0.5f;     // �U���G�t�F�N�g�̃T�C�Y
    status[1].death_effect_size = 0.5f;      // ���S�G�t�F�N�g�̃T�C�Y
    status[1].hit_effect_size = 0.5f;        // �ݒu�G�t�F�N�g�̃T�C�Y

    status[2].attack_power = 1;              //�U����
    status[2].attack_interval = 0.5f;        //�U���Ԋu
    status[2].health = 3;                    // HP
    status[2].speed_power = -0.5f;            // ���x
    status[2].radius = 0.5f;                 // ���a
    status[2].attack_effect_size = 0.5f;     // �U���G�t�F�N�g�̃T�C�Y
    status[2].death_effect_size = 0.5f;      // ���S�G�t�F�N�g�̃T�C�Y
    status[2].hit_effect_size = 0.5f;        // �ݒu�G�t�F�N�g�̃T�C�Y

    status[3].attack_power = 1;              //�U����
    status[3].attack_interval = 0.5f;        //�U���Ԋu
    status[3].health = 3;                    // HP
    status[3].speed_power = -0.5f;           // ���x
    status[3].radius = 0.5f;                 // ���a
    status[3].attack_effect_size = 0.5f;     // �U���G�t�F�N�g�̃T�C�Y
    status[3].death_effect_size = 0.5f;      // ���S�G�t�F�N�g�̃T�C�Y
    status[3].hit_effect_size = 0.5f;        // �ݒu�G�t�F�N�g�̃T�C�Y

    // EnemyB
    mover_timer_max_B = 1.0f; // ��C�ɐi�ނ܂ł̎���
    dis_B             = 3.0f;     // ��C�ɐi�ދ���

    // �Ƃ肠�����A�j���[�V����
    PlayAnimation(Animation::Move, true);
}

void SummonEnemy::DrawDebugPrimitive()
{
    DebugRenderer* debug_renderer = Lemur::Graphics::Graphics::Instance().GetDebugRenderer();
    debug_renderer->DrawCylinder(position, radius, height, { 0,1,0,1 });
}

void SummonEnemy::DrawDebugGUI(int n)
{
    Character::DrawDebugGUI("SummonEnemy", n);
}

void SummonEnemy::UpdateAttackState(float elapsed_time)
{
}

void SummonEnemy::UpdateMoveState(float elapsed_time)
{
    timer += elapsed_time;

    // �^�C�}�[������̎��ԂɂȂ�����G������
    if (attack_interval < timer)
    {
        Enemy* enemy = nullptr;
        switch (rand() % 3)
        {
        case EnemyType::A:
        {
            enemy = new Enemy_A(false);
            EnemySpawner::SetBasicEnemyStatus(enemy, status[rand() % 4]);//�����_���ȃX�e�[�^�X���Z�b�g

            enemy->SetShaft(rand() % 2);// �����_���Ȏ���
            enemy->SetPosition(position);// �����G�̍��W��ݒ�
            enemy->UpdateTransform();

            // ���X�g�ɒǉ�
            EnemyManager::Instance().Register(enemy);
            break;
        }
        case  EnemyType::B:
        {
            enemy = new Enemy_B(false);
            EnemySpawner::SetBasicEnemyStatus(enemy, status[rand() % 4]);//�����_���ȃX�e�[�^�X���Z�b�g
            enemy->SetMoveTimerMax(mover_timer_max_B);
            enemy->SetDis(dis_B);

            enemy->SetShaft(rand() % 2);// �����_���Ȏ���
            enemy->SetPosition(position);// �����G�̍��W��ݒ�
            enemy->UpdateTransform();

            // ���X�g�ɒǉ�
            EnemyManager::Instance().Register(enemy);
            break;
        }
        case   EnemyType::C:
        {
            enemy = new Enemy_C(false);
            EnemySpawner::SetBasicEnemyStatus(enemy, status[rand() % 4]);//�����_���ȃX�e�[�^�X���Z�b�g

            enemy->SetShaft(rand() % 2);// �����_���Ȏ���
            enemy->SetPosition(position);// �����G�̍��W��ݒ�
            enemy->UpdateTransform();

            // ���X�g�ɒǉ�
            EnemyManager::Instance().Register(enemy);
        }
        };

        timer = 0.0f;
    }

    // ���S�X�e�[�g��
    if (IsDead())
    {
        TransitionDeathState();
    }
}



BossEnemy::BossEnemy(bool is_minor)
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    if (!is_minor)LoadFBXModel(graphics.GetDevice(), ".\\resources\\Model\\Enemy\\Bear.fbx");
    else LoadFBXModel(graphics.GetDevice(), ".\\resources\\Model\\Enemy\\Bear2.fbx");

    attack_effect = new Effect(".\\resources\\Effect\\fence_break\\fence_break.efk");
    death_effect = new Effect(".\\resources\\Effect\\ENEMY_DOWN\\ENEMY_DOWN.efk");
    hit_effect = new Effect(".\\resources\\Effect\\hit_to_enemy\\hit_to_enemy.efk");

    attack_power = 1;        // �U����
    attack_interval = 3.0f;     // �U���Ԋu

    health = 10;       // HP
    radius = 1.0f;     // ���a
    height = 1.0f;     // �f�o�b�O�p
    speed_power = -1.0f;    // ���x

    // TODO ���ˁ@�傫��
    model_scale = 2.0f;;

    // �Ƃ肠�����A�j���[�V����
    PlayAnimation(Animation::Move, true);
}

void BossEnemy::DrawDebugPrimitive()
{
    DebugRenderer* debug_renderer = Lemur::Graphics::Graphics::Instance().GetDebugRenderer();
    debug_renderer->DrawCylinder(position, radius, height, { 0,1,0,1 });
}
void BossEnemy::DrawDebugGUI(int n)
{
    Character::DrawDebugGUI("BossEnemy", n);
}

void BossEnemy::UpdateAttackState(float elapsed_time)
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
            PlayAnimation(Animation::Attack, false);
            attack_effect->Play(position, attack_effect_size);
            // �^�C�}�[����
            timer = 0.0f;
        }
    }

    //TODO ����̓A�j���[�V������������v�ύX
    if (!IsPlayAnimation())
    {
        PlayAnimation(7, true);
    }

    // ���S���͎��S�X�e�[�g�֐؂�ւ�
    if (IsDead())
    {
        TransitionDeathState();
    }
}

void BossEnemy::UpdateMoveState(float elapsed_time)
{
    if (shaft == Shaft::Side) rotation.y = DirectX::XMConvertToRadians(-90);
    else if (shaft == Shaft::Vertical)rotation.y = DirectX::XMConvertToRadians(180);

    // ��ɐG���΍U���X�e�[�g��
    if (Collision::IntersectRectVsCircle(Fence::Instance().GetLeftRect(), { position.x,position.z }, radius) ||
        Collision::IntersectRectVsCircle(Fence::Instance().GetFrontRect(), { position.x,position.z }, radius))
    {
        TransitionAttackState();
    }
    else
    {
        // �ړ�
        if (shaft == Shaft::Side) velocity.x = speed_power;
        else if (shaft == Shaft::Vertical)velocity.z = speed_power;
    }

    // ���S�X�e�[�g��
    if (IsDead())
    {
        TransitionDeathState();
    }
}