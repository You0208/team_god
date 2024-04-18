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

    sub_pos_z = -StageManager::Instance().GetStage(StageManager::Instance().GetStageIndex())->GetVariableStageWidth().y-1.0f;

    // ���W�����Z
    position.z = sub_pos_z;

    // ���E�[
    limit = { StageManager::Instance().GetStage(StageManager::Instance().GetStageIndex())->GetStageCollision().left_up.x + 0.5f,
         StageManager::Instance().GetStage(StageManager::Instance().GetStageIndex())->GetStageCollision().right_down.x - 0.5f };

    unit_category = 1;
    // �Ƃ肠�����A�j���[�V����
    model->PlayAnimation(Animation::Idle, true);
}

Player::~Player()
{
}

// �X�V����
void Player::Update(float elapsedTime)
{
    sub_pos_z = StageManager::Instance().GetStage(StageManager::Instance().GetStageIndex())->GetStageCollision().right_down.y - 1.0f;

    // ���W�����Z
    position.z = sub_pos_z;

    // ���E�[
    limit = { StageManager::Instance().GetStage(StageManager::Instance().GetStageIndex())->GetStageCollision().left_up.x + 0.5f,
         StageManager::Instance().GetStage(StageManager::Instance().GetStageIndex())->GetStageCollision().right_down.x - 0.5f };


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

    // �ړ��̐���
    if (position.x <= limit.x)
    {
        position.x = limit.x;
    }
    else if (position.x >= limit.y)
    {
        position.x = limit.y;
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
        model->PlayAnimation(Animation::Pull, false);
        velocity.x = 0;

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
            model->PlayAnimation(Animation::Throw, false);
            velocity.x = 0;

            // �͂����������Z�o
            f_d = (s_l_max) / timer_s * 2.0f;
            // ������
            s_l_max = 0;
            timer_s = 0;

            // �X�P�[�����O
            float scaling = StageManager::Instance().GetStage(StageManager::Instance().GetStageIndex())->GetVariableStageWidth().y * 2;
            f_d = scaling * (f_d / scaling);
            // �ŏ��l0�A�ő�lscaling�ɃN�����v����
            f_d = std::clamp(f_d, 0.2f, scaling);
        }
    }

    //TODO ���̏����͖\�������˂Ȃ��̂ŗv�C��
    if (f_d >= 0.1f)
    {
        velocity.x = 0;
        Seed* seed = new Seed();
        DirectX::XMFLOAT2 unit_pos;
        if (!CollisionManager::CollisionUnitBackVsSeed_Re({ position.x ,f_d + sub_pos_z/*�͂����ŏo�����W����A�X�e�[�W�̔��a�����Z*/ }, unit_pos))
        {
            Seed* seed = new Seed();

            // ���W���m��
            seed->SetPosition(position.x/*�v���C���[��X���W*/, 0, position.z);
            seed->SetDistinationPosition({ position.x/*�v���C���[��X���W*/, 0, f_d + sub_pos_z });

            // ���j�b�g�����Ȃ��Ȃ瑦���ɔ���
            if (UnitManager::Instance().GetUnitCount() == 0)    seed->SetBorn(true);

            // ��̎�ނ�o�^
            seed->SetCategory(unit_category);

            seed->SetIsDirection(true);

            seed->UpdateTransform();

            // ���X�g�ɒǉ�
            SeedManager::Instance().Register(seed);
            // �͂���������������
            f_d = 0.0f;
        }
        else
        {
            // ���W���m��
            seed->SetPosition(position.x/*�v���C���[��X���W*/, 0, position.z);
            seed->SetDistinationPosition({ position.x/*�v���C���[��X���W*/, 0, unit_pos.y });

            seed->UpdateTransform();

            seed->SetIsDisDirection(true);
            // ���X�g�ɒǉ�
            SeedManager::Instance().Register(seed);
            f_d = 0.0f;
        }
    }

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
