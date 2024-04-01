#include "Player.h"
#include "Lemur/Input/Input.h"
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
    // ��I��
    for (Seed* seed : seeds)
    {
        delete seed;
    }
    seeds.clear();
}

// �X�V����
void Player::Update(float elapsedTime)
{
    // ��X�V
    SeedUpdate(elapsedTime);

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

    // ��̕`��
    for (Seed* seed : seeds)
    {
        seed->Render(scale, replaced_pixel_shader);
    }
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
        // ��ԍ�������
        seed->SetNumber(seed_number);
        // ���W���m��
        seed->SetPosition(position.x/*�v���C���[��X���W*/, 0, f_d + sub_pos_z/*�͂����ŏo�����W����A�X�e�[�W�̔��a�����Z*/);
        // ���X�g�ɒǉ�
        seeds.emplace_back(seed);
        // ��ԍ���i�߂�
        seed_number++;
        // �͂���������������
        f_d = 0.0f;
    }

}

// �͂����X�V
void Player::SeedUpdate(float elapsedTime)
{
    // ���Ԑ؂�̎�������iseed->death��true�̂Ƃ��j
    seeds.erase(std::remove_if(seeds.begin(), seeds.end(), [](Seed* seed) {return seed->death; }), seeds.end());

    for (Seed* seed : seeds)
    {
        // ���Ԑ؂�̎�
        if (seed->GetTimer() >= 15.0f)
        {
            // ���S�ɐ؂�ւ�
            seed->death = true;

            for (Seed* s : seeds)
            {
                // �����폜�\��̎�ԍ������傫���ԍ��̎킪����΁A�ԍ������Z����
                if (seed->GetNumber() < s->GetNumber())s->DecNumber();
            }
            // ���݂̎�ԍ������Z
            seed_number--;
        }
        // ��̍X�V
        seed->Update(elapsedTime);
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
