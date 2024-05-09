#pragma once
#include "Enemy.h"
#include "EnemySpawner.h"
class SummonEnemy :public Enemy
{
public:
    SummonEnemy(bool is_minor);
    ~SummonEnemy()override {};
    // �f�o�b�O�v���~�e�B�u�`��
    void DrawDebugPrimitive()override;
    void DrawDebugGUI(int n)override;
private:
    // �U��
    void UpdateAttackState(float elapsed_time)override;
    // �ړ�
    void TransitionMoveState()override {}
    void UpdateMoveState(float elapsed_time)override;
private:
    float timer;// �^�C�}�[
    EnemySpawner::EnemyStatus status[4];
    float mover_timer_max_B = 1.0f; // ��C�ɐi�ނ܂ł̎���
    float dis_B = 3.0f;     // ��C�ɐi�ދ���
};

class BossEnemy :public Enemy
{
public:
    BossEnemy(bool is_minor);
    ~BossEnemy()override {};
    // �f�o�b�O�v���~�e�B�u�`��
    void DrawDebugPrimitive()override;
    void DrawDebugGUI(int n)override;
private:
    // �U��
    void UpdateAttackState(float elapsed_time)override;
    // �ړ�
    void TransitionMoveState()override {}
    void UpdateMoveState(float elapsed_time)override;
private:
    float timer;// �^�C�}�[
};