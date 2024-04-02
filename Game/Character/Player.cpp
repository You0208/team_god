#include "Player.h"
#include "Lemur/Input/Input.h"
#include "Game/CollisionManager.h"
#include "SeedManager.h"
#include "UnitManager.h"
static Player* instance = nullptr;

Player::Player()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();

    // ���f���̏�����
    model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources_2\\Model\\nico.fbx");

    // ���W�����Z
    position.z = sub_pos_z;

    // �Ƃ肠�����A�j���[�V����
    model->PlayAnimation(0, true);
}

Player::~Player()
{
}

// �X�V����
void Player::Update(float elapsedTime)
{
    // �͂�������
    Flick(elapsedTime);

    // ���͏���
    InputProcess();

    // ���͏����X�V
    UpdateVelocity(elapsedTime);

    // ���I���ԍX�V
    UpdateInvincibleTimer(elapsedTime);

    // ���f���A�j���[�V�����X�V
    model->UpdateAnimation(elapsedTime);

    // �s��X�V����
    UpdateTransform();

    // Imgui
    DrawDebugGUI();

}

// �`�揈��
void Player::Render(float scale, ID3D11PixelShader** replaced_pixel_shader)
{
    // �f�o�b�O�\��
    model->DrawDebug("Player");
    // ���������f���`��
    model->Render(scale, replaced_pixel_shader);
}

// Imgui
void Player::DrawDebugGUI()
{
    ImGui::Begin("Player");

    ImGui::SliderFloat("moveSpeed", &moveSpeed, 0.0f, 10.0f);
    ImGui::SliderFloat("f_d", &f_d, 0.0f, 10.0f);
    
    ImGui::End();
}

// �͂�������
void Player::Flick(float elapsedTime)
{
    GamePad& gamePad = Input::Instance().GetGamePad();

    // �R���g���[���[�̉E�X�e�B�b�NY����
    s_l = gamePad.GetAxisRY() * -1.0f;

    // �E�X�e�B�b�N���������ꂽ�Ƃ�
    if (s_l > 0.1f)
    {
        // �^�C�}�[�𓮂���
        timer_s += elapsedTime;
        // �ő�l���X�V��������
        if (s_l_max <= s_l)s_l_max = s_l;
    }
    else
    {
        // �^�C�}�[��0�ȏぁ�E�X�e�B�b�N�������ꂽ��
        if (timer_s > 0)
        {
            // �͂����������Z�o
            f_d = (s_l_max) / timer_s;
            // ������
            s_l_max = 0;
            timer_s = 0;
        }
    }

    //TODO ���̏����͖\�������˂Ȃ��̂ŗv�C��
    if (f_d >= 0.1f)
    {
        Seed* seed = new Seed();
        // ���j�b�g�����Ȃ��Ȃ瑦���ɔ���
        if (UnitManager::Instance().GetUnitCount() == 0)seed->SetBorn(true);
        // ��̎�ނ�o�^
        seed->SetCategory(unit_category);

        // ���j�b�g�̉��ɍs���Ȃ��悤��
        f_d = CollisionManager::Instance().CollisionUnitBackVsSeed({ position.x ,f_d + sub_pos_z/*�͂����ŏo�����W����A�X�e�[�W�̔��a�����Z*/ }).y;

        // ���W���m��
        seed->SetPosition(position.x/*�v���C���[��X���W*/, 0, f_d);
        // ���X�g�ɒǉ�
        SeedManager::Instance().Register(seed);
        // �͂���������������
        f_d = 0.0f;
    }

}

// ���͏���
void Player::InputProcess()
{
    GamePad& gamePad = Input::Instance().GetGamePad();

    // ���X�e�B�b�NX�������X�s�[�h�ɕϊ�
    velocity.x = gamePad.GetAxisLX() * moveSpeed;
}

Player& Player::Instance()
{
    return *instance;
}
