#pragma once
#include "Enemy.h"
class Enemy_D :public Enemy
{
public:
    Enemy_D();
    ~Enemy_D()override {};

    // �X�V����
    void Update(float elapsedTime) override;

    // �`�揈��
    void Render(float elapsedTime, ID3D11PixelShader** replaced_pixel_shader) override;

    // �f�o�b�O�v���~�e�B�u�`��
    void DrawDebugPrimitive()override;

private:
    // �U���X�V
    void AttackUpdate(float elapsedTime) override;

    // �ړ��X�V
    void MoveUpdate(float elapsedTime) override;

    // Imgui
    void DrawDebugGUI()override;
    // ���S����
    void OnDead()override;

private:
    int move_state;             // �ړ��̏��
    float dis;                  // �ړ���
    float dis_max;              // �ړ��ʋK��l
    float speed_power_Y = 10.0f;// Y�����̈ړ���
    int direction_state = 0;    // �����̏��
    bool is_last_touched;       // �O��̈ړ��ō�ɐG�ꂽ��
    
    float attack_timer;         // �U���Ԋu
};

