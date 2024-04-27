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

    // �C���X�^���X�擾
    static Player& Instance();
private:
    // ���͏���
    void InputProcess();

    // �͂�������
    void Flick(float elapsedTime);

    //void FlickRe(float elapsedFrame);

    // �J�e�S���[�̕ύX
    void ChangeCategory();
private:
    enum Animation
    {
        Pull,
        Throw,
        Right,
        Left,
        Idle
    };
    enum UnitCategory
    {
        A,
        B,
        C,
        D,
        E,
        F,
        H,
        I,
        J
    };

    float               sub_pos_z           = 0.0f;     // ���Z�����Z���W
    float               sub_pos_z_puls      = 0.0f;     // ���Z�����Z���W�̒ǉ���
    float               moveSpeed           = 5.0f;     // ���ړ��̑��x
    DirectX::XMFLOAT2   limit               = {};       // �ĎR�q�̍��E�[
    
    bool                is_direction        = false;    // ��̉��o�t���O
    bool                is_dis_direction    = false;    // ��̏Փˉ��o�t���O
    float               seed_throw_speed    = 0.0f;     // ��̎ˏo���x

    int                 unit_category       = 0;        // �I������Ă��郆�j�b�g�̎��
    
    // �R���g���[���[�p
    float               flip_speed          = 0.0f;     // ��������̑��x�i�����̋����̕ύX�j
    float               right_stick_y       = 0.0f;     // �R���g���[���[�̉E�X�e�B�b�NY
    float               flip_timer          = 0.0f;     // �R���g���[���[���͂������̂ɂ�����������
    float               max_right_stick_y   = 0.0f;     // �͂����ꂽ�E�X�e�B�b�NY���W�̍ő� 
    float               flip_pos_z          = 0.0f;     // �ŏI�I�ɓ����o���ꂽ�͂������W
    float               max_charge_time     = 2.0f;     // ���ߍU���̍ő厞��
    bool                is_throw            = false;    // ������ꂽ���ǂ���

};


