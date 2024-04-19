#include "Player.h"
#include "Lemur/Input/Input.h"
#include "Game/CollisionManager.h"
#include "SeedManager.h"
#include "UnitManager.h"
#include "Game/Stage/StageManager.h"

static Player* instance = nullptr;

Player::Player()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();

    // ���f���̏�����
    model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\scarecrow_Re.fbx");

    // �ĎR�q�̏����ʒu�C��
    sub_pos_z_puls = 1.0f;
    sub_pos_z = StageManager::Instance().GetStage(StageManager::Instance().GetStageIndex())->GetVariableStageWidth().y + sub_pos_z_puls;
    position.z = -sub_pos_z;

    // ���E�[������
    limit = { StageManager::Instance().GetStage(StageManager::Instance().GetStageIndex())->GetStageCollision().left_up.x + 0.5f,
         StageManager::Instance().GetStage(StageManager::Instance().GetStageIndex())->GetStageCollision().right_down.x - 0.5f };

    // ���j�b�g�J�e�S���[�̏�����
    unit_category = 0;

    // �͂�������
    flip_speed = 2.0f;

    // ��̎ˏo���x
    seed_throw_speed = 20.0f;

    // �Ƃ肠�����A�j���[�V����
    model->PlayAnimation(Animation::Idle, true);
}

Player::~Player()
{
}

// �X�V����
void Player::Update(float elapsedTime)
{
    // ��Ɏ��s
    {
        // ���͏����X�V
        UpdateVelocity(elapsedTime);

        // ���f���A�j���[�V�����X�V
        model->UpdateAnimation(elapsedTime);

        // �s��X�V����
        UpdateTransform();

        // Imgui
        DrawDebugGUI();
    }

    // ���͏���
    InputProcess();

    // �͂�������
    Flick(elapsedTime);

    // �J�e�S���[�̕ύX
    ChangeCategory();

    // �ĎR�q�ʒu�̍X�V
    sub_pos_z = StageManager::Instance().GetStage(StageManager::Instance().GetStageIndex())->GetVariableStageWidth().y + sub_pos_z_puls;
    position.z = -sub_pos_z;

    // ���E�[
    limit = { StageManager::Instance().GetStage(StageManager::Instance().GetStageIndex())->GetStageCollision().left_up.x + 0.5f,
         StageManager::Instance().GetStage(StageManager::Instance().GetStageIndex())->GetStageCollision().right_down.x - 0.5f };

    // �ړ��̐���
    position.x = (position.x <= limit.x) ? limit.x : (position.x >= limit.y) ? limit.y : position.x;
}

// Imgui
void Player::DrawDebugGUI()
{
    ImGui::Begin(u8"�ĎR�q�Ƃ�");

    ImGui::SliderFloat(u8"�ړ����x", &moveSpeed, 0.0f, 10.0f);
    ImGui::SliderFloat(u8"�X�e�[�W�Ƃ̋���", &sub_pos_z_puls, 0.0f, 5.0f);
    ImGui::SliderFloat(u8"��̎ˏo���x", &seed_throw_speed, 0.0f, 100.0f);
    ImGui::SliderFloat(u8"�͂������������ŕς���܂�", &flip_speed, 0.1f, 10.0f);
    ImGui::SliderFloat(u8"�R���g���[���[�������̂͂���", &flip_pos_z, 0.0f, 50.0f);
    
    ImGui::End();
}

// �͂�������
void Player::Flick(float elapsedTime)
{
    GamePad& gamePad = Input::Instance().GetGamePad();

    // �R���g���[���[�̉E�X�e�B�b�NY����
    right_stick_y = gamePad.GetAxisRY() * -1.0f;

    // �E�X�e�B�b�N���������ꂽ�Ƃ��i��������ꂽ���j
    if (right_stick_y > 0.1f)
    {
        // �������胂�[�V������
        model->PlayAnimation(Animation::Pull, false);
        // ���ړ��ł��Ȃ��悤��
        velocity.x = 0;

        // �^�C�}�[�𓮂���
        flip_timer += elapsedTime;
        // �ő�l���X�V��������
        if (max_right_stick_y <= right_stick_y)max_right_stick_y = right_stick_y;
    }
    else
    {
        // �^�C�}�[��0�ȏぁ�E�X�e�B�b�N�������ꂽ��
        if (flip_timer > 0)
        {
            // �����A�j���[�V������
            model->PlayAnimation(Animation::Throw, false);
            // ���ړ��ł��Ȃ��悤��
            velocity.x = 0;

            // �͂����������Z�o
            flip_pos_z = (max_right_stick_y) / flip_timer * flip_speed;
            // ������
            max_right_stick_y = 0;
            flip_timer = 0;

            // �X�P�[�����O
            float scaling = StageManager::Instance().GetStage(StageManager::Instance().GetStageIndex())->GetVariableStageWidth().y * 2;
            flip_pos_z = scaling * (flip_pos_z / scaling);
            // �ŏ��l0�A�ő�lscaling�ɃN�����v����
            flip_pos_z = std::clamp(flip_pos_z, 0.2f, scaling);
        }
    }

    //TODO ���̏����͖\�������˂Ȃ��̂ŗv�C��
    if (flip_pos_z >= 0.1f)
    {
        // ���ړ��o���Ȃ��悤��
        velocity.x = 0;
        // ��̎��̂𐶐�
        Seed* seed = new Seed();
        // ��ԋ߂����j�b�g�̏ꏊ��n��
        DirectX::XMFLOAT2 unit_pos;

        if (!CollisionManager::CollisionUnitBackVsSeed_Re({ position.x ,flip_pos_z - sub_pos_z/*�͂����ŏo�����W����A�X�e�[�W�̔��a�����Z*/ }, unit_pos))
        {
            // ���j�b�g�����Ȃ��Ȃ瑦���ɔ���
            if (UnitManager::Instance().GetUnitCount() == 0)    seed->SetBorn(true);
            // ���W���m��
            seed->SetPosition(position.x/*�v���C���[��X���W*/, 0, position.z);
            seed->SetDistinationPosition({ position.x/*�v���C���[��X���W*/, 0, flip_pos_z - sub_pos_z });
            // ��̎�ނ�o�^
            seed->SetCategory(unit_category);
            // ���o�A��
            seed->SetIsDirection(true);
            // ��̎ˏo���x�̐ݒ�
            seed->SetthrowSpeed(seed_throw_speed);
            // �p���X�V
            seed->UpdateTransform();
            // ���X�g�ɒǉ�
            SeedManager::Instance().Register(seed);

            // �͂���������������
            flip_pos_z = 0.0f;
        }
        else
        {
            // ���W���m��
            seed->SetPosition(position.x/*�v���C���[��X���W*/, 0, position.z);
            seed->SetDistinationPosition({ position.x/*�v���C���[��X���W*/, 0, unit_pos.y });
            // ��̎ˏo���x�̐ݒ�
            seed->SetthrowSpeed(seed_throw_speed);
            // ���o�I��
            seed->SetIsDisDirection(true);
            // �p���X�V
            seed->UpdateTransform();
            // ���X�g�ɒǉ�
            SeedManager::Instance().Register(seed);

            // �͂���������������
            flip_pos_z = 0.0f;
        }
    }

}

// �J�e�S���[�̕ύX
void Player::ChangeCategory()
{
    GamePad& gamePad = Input::Instance().GetGamePad();

    if (gamePad.GetButtonDown() & gamePad.BTN_A)unit_category = UnitCategory::A;
    else if (gamePad.GetButtonDown() & gamePad.BTN_B)unit_category = UnitCategory::B;
    else if (gamePad.GetButtonDown() & gamePad.BTN_X)unit_category = UnitCategory::C;
    else if (gamePad.GetButtonDown() & gamePad.BTN_Y)unit_category = UnitCategory::D;
}

// ���͏���
void Player::InputProcess()
{
    GamePad& gamePad = Input::Instance().GetGamePad();
    if ((model->GetCurrentAnimationIndex() == Animation::Throw|| model->GetCurrentAnimationIndex() == Animation::Pull) && model->IsPlayAnimation())
    {
        velocity.x = 0;
    }
    else
    {
        // ���X�e�B�b�NX�������X�s�[�h�ɕϊ�
        velocity.x = gamePad.GetAxisLX() * moveSpeed;
        if (velocity.x < 0)  model->PlayAnimation(Animation::Left, false);
        else if (velocity.x > 0)  model->PlayAnimation(Animation::Right, false);
        else if (velocity.x == 0)  model->PlayAnimation(Animation::Idle, false);
    }
}

Player& Player::Instance()
{
    return *instance;
}
