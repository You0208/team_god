#pragma once
#include "Lemur/Object/Character.h"
#include "Lemur/Graphics/Shader.h"
#include "Lemur/Model/FbxModelManager.h"
#include "Lemur/Effekseer/Effect.h"
#include "Seed.h"

class Player :public Character
{
public:
    Player();
    ~Player()override;

    // �X�V����
    void Update(float elapsedTime);

    // �`�揈��
    void Render(float scale, ID3D11PixelShader** replaced_pixel_shader);

    // Imgui
    void DrawDebugGUI();

    // �͂�������
    void Flick(float elapsedTime);

    // ��̍X�V����
    void SeedUpdate(float elapsedTime);

    // �C���X�^���X�擾
    static Player& Instance();
private:
    // ���͏���
    void InputProcess();

private:
    std::vector<Seed*> seeds;// ��̃��X�g
    float seed_limit_time = 15.0f;// ��̐�������
    int seed_number = 0;// ���̎�ԍ�

    float moveSpeed = 5.0f;// ���ړ��̑��x
    float sub_pos_z = -5.0f;// ���Z�����Z���W

    int unit_category = 0;// �I������Ă��郆�j�b�g�̎��

    // �R���g���[���[�p
    float s_l;// �R���g���[���[�̉E�X�e�B�b�NY
    float timer_s;// �R���g���[���[���͂������̂ɂ�����������
    float s_l_max;//�͂����ꂽ�E�X�e�B�b�NY���W�̍ő� 
    float f_d = 0;// �ŏI�I�ɓ����o���ꂽ�͂������W
};


