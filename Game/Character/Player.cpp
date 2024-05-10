#include "Player.h"
#include "Lemur/Input/Input.h"
#include "Game/CollisionManager.h"
#include "SeedManager.h"
#include "UnitManager.h"
#include "Game/Stage/StageManager.h"
#include "Game/Character/UnitManager.h"
#include "./Lemur/Scene/SceneManager.h"

static Player* instance = nullptr;

Player::Player()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();

    // ���f���̏�����
    LoadFBXModel(graphics.GetDevice(), ".\\resources\\Model\\scarecrow_Re.fbx");

    // ���E�[������
    limit = { StageManager::Instance().GetStage(StageManager::Instance().GetStageIndex())->GetStageCollision().left_up.x + 0.5f,
         StageManager::Instance().GetStage(StageManager::Instance().GetStageIndex())->GetStageCollision().right_down.x - 0.5f };

    // ���j�b�g�J�e�S���[�̏�����
    unit_category = UnitManager::UNIT_INDEX::GreenPumpkin;

    //TODO ���ˁ@�Q�[�����
    {
        // �͂�������
        flip_speed = 2.0f;
        // ��̎ˏo���x
        seed_throw_speed = 21.0f;
        // �ĎR�q���򂩂痣��鋗��
        sub_pos_z_puls = 0.55f;
        // �ĎR�q�̈ړ����x
        moveSpeed = 5.1f;
        // �ĎR�q�����̍ŒZ����
        dis_scarecrow = 1.0f;
    }
    // �ĎR�q�̏����ʒu�C��
    sub_pos_z = StageManager::Instance().GetStage(StageManager::Instance().GetStageIndex())->GetVariableStageWidth().y + sub_pos_z_puls;
    position.z = -sub_pos_z;
    // �Ƃ肠�����A�j���[�V����
    PlayAnimation(Animation::Idle, true);
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
        UpdateAnimation(elapsedTime);

        // �s��X�V����
        UpdateTransform();

        // Imgui
        DrawDebugGUI();
    }

    // ���͏���
    InputProcess();

    // �o�߃t���[��
    float elapsed_frame = 60.0f * elapsedTime;
   // FlickRe(elapsed_frame);
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
    ImGui::SliderFloat(u8"���߂̍ő厞��", &max_charge_time, 0.0f, 3.0f);
    ImGui::SliderFloat(u8"�킪�������Ԏ�O", &dis_scarecrow, 0.0f, 3.0f);
    
    ImGui::End();
}

// �͂�������
void Player::Flick(float elapsedTime)
{
    GamePad& gamePad = Input::Instance().GetGamePad();

    // �R���g���[���[�̉E�X�e�B�b�NY����
    right_stick_y = gamePad.GetAxisRY() * -1.0f;

    // �E�X�e�B�b�N���������ꂽ�Ƃ��i��������ꂽ���j
    if ( right_stick_y > 0.1f)
    {
        // �������胂�[�V������
        // ���ړ��ł��Ȃ��悤��
        velocity.x = 0;
        PlayAnimation(Animation::Pull, false);

        if (flip_timer > 0.5f)
        {
            //// �����A�j���[�V������
            PlayAnimation(Animation::Throw, false);
            //// ���ړ��ł��Ȃ��悤��
            //velocity.x = 0;

            // �^�C�}�[�𓮂���
            flip_timer += elapsedTime;
            // �ő�l���X�V��������
            if (max_right_stick_y <= right_stick_y)max_right_stick_y = right_stick_y;
        }
        else
        {
            // �^�C�}�[�𓮂���
            flip_timer += elapsedTime;
            // �ő�l���X�V��������
            if (max_right_stick_y <= right_stick_y)max_right_stick_y = right_stick_y;
        }
    }
    else
    {
        // �^�C�}�[��0�ȏぁ�E�X�e�B�b�N�������ꂽ��
        if (flip_timer > 0)
        {
            // �����A�j���[�V������
            PlayAnimation(Animation::Throw, false);
            // ���ړ��ł��Ȃ��悤��
            velocity.x = 0;

            // �͂����������Z�o
            flip_pos_z = (max_right_stick_y) / flip_timer * flip_speed;
            // ������
            if (flip_timer > max_charge_time)
            {
                flip_pos_z = sub_pos_z_puls;
            }

            // �X�P�[�����O
            float scaling = StageManager::Instance().GetStage(StageManager::Instance().GetStageIndex())->GetVariableStageWidth().y * 2;
            // �ŏ��l0�A�ő�lscaling�ɃN�����v����
            flip_pos_z = std::clamp(flip_pos_z + dis_scarecrow, dis_scarecrow, scaling);
            flip_pos_z = (scaling + sub_pos_z_puls+ dis_scarecrow) - flip_pos_z;

            max_right_stick_y = 0;
            right_stick_y = 0;
            flip_timer = 0;

            is_throw = true;
        }
    }

    //TODO ���̏����͖\�������˂Ȃ��̂ŗv�C��
    if (is_throw)
    {
        is_throw = false;
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

    // TODO ���ˁ@���j�b�g�̃{�^���ݒ�
    Lemur::Scene::SceneManager::Instance().set_unit_cont[gamePad.B]= UnitManager::UNIT_INDEX::Chili;
    Lemur::Scene::SceneManager::Instance().set_unit_cont[gamePad.A]= UnitManager::UNIT_INDEX::Cauliflower;
    Lemur::Scene::SceneManager::Instance().set_unit_cont[gamePad.X]= UnitManager::UNIT_INDEX::Shishito;
    Lemur::Scene::SceneManager::Instance().set_unit_cont[gamePad.Y]= UnitManager::UNIT_INDEX::GreenPumpkin;

    if (gamePad.GetButtonDown() & gamePad.BTN_B)unit_category = Lemur::Scene::SceneManager::Instance().set_unit_cont[gamePad.B];
    else if (gamePad.GetButtonDown() & gamePad.BTN_A)unit_category = Lemur::Scene::SceneManager::Instance().set_unit_cont[gamePad.A];
    else if (gamePad.GetButtonDown() & gamePad.BTN_X)unit_category =  Lemur::Scene::SceneManager::Instance().set_unit_cont[gamePad.X];
    else if (gamePad.GetButtonDown() & gamePad.BTN_Y)unit_category =  Lemur::Scene::SceneManager::Instance().set_unit_cont[gamePad.Y];
}

// ���͏���
void Player::InputProcess()
{
    GamePad& gamePad = Input::Instance().GetGamePad();
    if ((GetCurrentAnimationIndex() == Animation::Throw|| GetCurrentAnimationIndex() == Animation::Pull) && IsPlayAnimation())
    {
        velocity.x = 0;
    }
    else
    {
        // ���X�e�B�b�NX�������X�s�[�h�ɕϊ�
        velocity.x = gamePad.GetAxisLX() * moveSpeed;
        if (velocity.x < 0)  PlayAnimation(Animation::Left, false);
        else if (velocity.x > 0)  PlayAnimation(Animation::Right, false);
        else if (velocity.x == 0)  PlayAnimation(Animation::Idle, false);
    }
}

Player& Player::Instance()
{
    return *instance;
}
