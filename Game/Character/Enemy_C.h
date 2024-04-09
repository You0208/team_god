#pragma once
#include "Enemy.h"
class Enemy_C :public Enemy
{
public:
    Enemy_C();
    ~Enemy_C()override {};

    // �X�V����
    void Update(float elapsedTime) override;

    // �`�揈��
    void Render(float elapsedTime, ID3D11PixelShader** replaced_pixel_shader) override;

    // �U���X�V
    void AttackUpdate(float elapsedTime) override;

    // �ړ��X�V
    void MoveUpdate(float elapsedTime) override;

    void DrawDebugGUI()override;

    // �f�o�b�O�v���~�e�B�u�`��
    void DrawDebugPrimitive()override;

    // ���S����
    void OnDead()override;

    //TODO ���j�b�g�ɐڐG�������i���j�b�g�̕��@�����蔻��ɓZ�߂��Ȃ����j
    void JudgeUnit();
private:
    enum StateIndex
    {
        Move_State,
        Attack_State
    };

    DirectX::XMFLOAT2 destination = { 0,0 };

    float timer = 0.0f;
    int move_state = 0;
    bool is_touched_unit = false;
};

