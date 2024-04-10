#pragma once
#include "Lemur/Object/Character.h"
#include "Lemur/Graphics/Shader.h"
#include "../Stage/Fence.h"


class Enemy :public Character
{
public:
    Enemy() {}
    ~Enemy() {}

    // �X�V����
    virtual void Update(float elapsedTime) = 0;

    // �`�揈��
    virtual void Render(float elapsedTime, ID3D11PixelShader** replaced_pixel_shader) = 0;

    // �f�o�b�O�v���~�e�B�u�`��
    virtual void DrawDebugPrimitive();

    // �j��
    void Destroy();

protected:
    // �ړ��X�e�[�g�X�V
    virtual void MoveUpdate(float elapsed_time) {};

    // �U���X�e�[�g�X�V
    virtual void AttackUpdate(float elapsed_time) {};

protected:
    enum StateIndex
    {
        Move_State,
        Attack_State
    };

    bool is_hit_unit = false;// �v���C���[�ɓ���������    
    int state_index;// �X�e�[�g
};