#pragma once
#include "Lemur/Object/Character.h"
#include "Lemur/Effekseer/Effect.h"

class Seed :public Character
{
public:
    Seed();
    ~Seed()override;

    void Update(float elapsedTime);

    void throwDirection();
    void DisDirection();

    // �`�揈��
    void Render(float scale, ID3D11PixelShader* replaced_pixel_shader);

    void DrawDebugGUI();

    // �f�o�b�O�v���~�e�B�u�`��
    void DrawDebugPrimitive();

    // ��΂����Ă̎�̈ʒu����
    void DecisionPos();

    // �����_���ȃx�N�g��
    DirectX::XMFLOAT2 GenerateRandomDirection();

    // �w�肵�������������ړ�����֐�
    DirectX::XMFLOAT2 MoveInRandomDirection(const DirectX::XMFLOAT2& position, float distance);


    float   GetTimer() { return timer; };
    int     GetNumber() { return number; };
    bool    GetBorn() { return born; };
    bool    GetDesitionPos() { return decision_pos; }

    void    SetDistinationPosition(DirectX::XMFLOAT3 distination_position_) { distination_position = distination_position_; }
    void    SetNumber(int number_) { number= number_; }
    void    SetCategory(int category_) { category = category_; }
    void    SetthrowSpeed(float throw_speed_) { throw_speed = throw_speed_; }
    void    SetBorn(bool born_) { born = born_; }
    void    SetDesitionPos(bool decision_pos_) { decision_pos = decision_pos_; }
    void    SetIsDirection(bool is_direction_) { is_direction = is_direction_; }
    void    SetIsDisDirection(bool is_dis_direction_) { is_dis_direction = is_dis_direction_; }

    void    DecNumber() { number--; }

    Effect* error_effect = nullptr;
    Effekseer::Handle error_effect_handle = 0;
private:
    enum Animation_Index
    {
        Stop,
        Idle,
        Jump,
        Throw
    };

    float               timer                   = 0.0f;            // �킪�z�u����Ă���̎���
    int                 number                  = 0;               // ��ԍ�
    int                 category                = 0;               // ��̎��
    bool                born                    = false;           // ���j�b�g�����܂�邩�ǂ���
    bool                decision_pos            = false;           // ���j�b�g�̈ʒu�����肳��Ă��邩
    int                 overlap_num             = 0;               // ����d�Ȃ�����
    DirectX::XMFLOAT2   outPosition             = {};              // �����o���ʒu
    DirectX::XMFLOAT3   distination_position    = {};              // �ړI�n

    bool                is_direction            = false;           // ��̓������o
    bool                is_dis_direction        = false;           // ��̏����鉉�o
    float               throw_speed             = 20.0f;           // ��̑��x
    float               effect_size             = 0.0f;            // �G�t�F�N�g�̃T�C�Y

    float               seed_fence_dis          = 0.5f;            // ��ƍ�̋����̋��e�͈�
};


