#pragma once
#include "Lemur/Object/Character.h"
#include "Lemur/Graphics/Shader.h"
#include "Lemur/Effekseer/Effect.h"
#include "Seed.h"

class Player :public Character
{
public:
    Player();
    ~Player()override;

    // �X�V����
    void Update(float elapsedTime);

    // Imgui
    void DrawDebugGUI();

    // �͂�������
    void Flick(float elapsedTime);

    // �C���X�^���X�擾
    static Player& Instance();
private:
    // ���͏���
    void InputProcess();

private:
    enum Animation
    {
        Pull,
        Throw,
        Right,
        Left,
        Idle
    };
    float moveSpeed = 5.0f;// ���ړ��̑��x
    float sub_pos_z = -5.0f;// ���Z�����Z���W

    int unit_category = 0;// �I������Ă��郆�j�b�g�̎��

    bool is_throw_animation;// ������A�j���[�V�����Đ�����

    // �R���g���[���[�p
    float s_l;// �R���g���[���[�̉E�X�e�B�b�NY
    float timer_s;// �R���g���[���[���͂������̂ɂ�����������
    float s_l_max;//�͂����ꂽ�E�X�e�B�b�NY���W�̍ő� 
    float f_d = 0;// �ŏI�I�ɓ����o���ꂽ�͂������W


    DirectX::XMFLOAT2 limit;
};


