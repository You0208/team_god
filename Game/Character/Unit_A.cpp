#include "Unit_A.h"
#include "Lemur/Input/Input.h"

static Unit_A* instance = nullptr;

Unit_A::Unit_A()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    model_unit_A = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources_2\\Model\\Jummo\\Jummo.fbx");

    scale.x = scale.y = scale.z = 0.01f;
    radius = 0.5f;
    position = { 0,5.0f,0 };
    height = 1.0f;

    instance = this;
}

Unit_A::~Unit_A()
{
}

void Unit_A::Update(float elapsedTime)
{
    // �X�e�[�g���Ƃ̏���
    switch (state)
    {
    case State::Idle:
        UpdateIdleState(elapsedTime);
        break;
    case State::Move:
        UpdateMoveState(elapsedTime);
        break;
    case State::Jump:
        UpdateJumpState(elapsedTime);
        break;
    case State::Fall:
        UpdateFallState(elapsedTime);
        break;
    case State::Land:
        UpdateLandState(elapsedTime);
        break;
    case State::Attack:
        UpdateAttackState(elapsedTime);
        break;
    case State::Damage:
        UpdateDamageState(elapsedTime);
        break;
    case State::Death:
        UpdateDeathState(elapsedTime);
        break;
    case State::Revive:
        UpdateReviveState(elapsedTime);
        break;
    }

    // ���͏����X�V
    UpdateVelocity(elapsedTime);

    // ���I���ԍX�V
    UpdateInvincibleTimer(elapsedTime);

    // �v���C���[�ƓG�Ƃ̏Փ˔���
    CollisionPlayerVsEnemies();

    UpdateTransform();

    // ���f���A�j���[�V�����X�V����
    model_unit_A->UpdateAnimation(elapsedTime);

}

void Unit_A::Render(float elapsedTime, ID3D11PixelShader** replaced_pixel_shader)
{
    model_unit_A->Render(elapsedTime, replaced_pixel_shader);
}

void Unit_A::DrawDebugGUI()
{
    if (ImGui::TreeNode("Unit_A"))
    {
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // �ʒu
            ImGui::InputFloat3("position", &position.x);

            // ��]
            DirectX::XMFLOAT3 a;
            a.x = DirectX::XMConvertToDegrees(rotation.x);
            a.y = DirectX::XMConvertToDegrees(rotation.y);
            a.z = DirectX::XMConvertToDegrees(rotation.z);
            ImGui::InputFloat3("rotation", &a.x);
            rotation.x = DirectX::XMConvertToRadians(a.x);
            rotation.y = DirectX::XMConvertToRadians(a.y);
            rotation.z = DirectX::XMConvertToRadians(a.z);

            // �X�P�[��
            ImGui::InputFloat3("Scale", &scale.x);

            // ���F���V�e�B
            ImGui::InputFloat3("velocity", &velocity.x);
            // ���n���Ă邩
            ImGui::Checkbox("isGround", &isGround);
            // �W�����v�J�E���g
            ImGui::InputInt("jumpCount", &jumpCount);

            // ���˃��[�g
            ImGui::InputFloat("LuanchRate", &LuanchRate);
            // ���˃N�[���^�C��
            ImGui::InputFloat("LuanchCollTime", &LuanchCollTime);
        }
        ImGui::TreePop();
    }
}

void Unit_A::DrawDebugPrimitive()
{
}


Unit_A& Unit_A::Instance()
{
    return *instance;
}

void Unit_A::TransitionIdleState()
{
    state = State::Idle;

    model_unit_A->PlayAnimation(Anim_Idle, true);
}

void Unit_A::UpdateIdleState(float elapsedTime)
{
    // �ړ����͏���
    if (InputMove(elapsedTime))
    {
        TransitionMoveState();
    }


    // �e�ۓ��͏���
    LuanchProjectile(elapsedTime);

    // �U�����͏���
    if (InputAttack())
    {
        TransitionAttackState();
    }
}

void Unit_A::TransitionMoveState()
{
    state = State::Move;

    // ����A�j���[�V�����Đ�
    model_unit_A->PlayAnimation(Anim_Running, true);
}

void Unit_A::UpdateMoveState(float elapsedTime)
{
    // �ړ����͂��Ȃ��Ȃ�����ҋ@�ɐ؂�ւ�
    if (!InputMove(elapsedTime))
    {
        TransitionIdleState();
    }


    // �e�ۓ��͏���
    LuanchProjectile(elapsedTime);

    // �U�����͏���
    if (InputAttack())
    {
        TransitionAttackState();
    }
}

void Unit_A::TransitionJumpState()
{
    state = State::Jump;

    // �W�����v�A�j���[�V�����Đ�
    model_unit_A->PlayAnimation(Anim_Jump, false);
}

void Unit_A::UpdateJumpState(float elapsedTime)
{
    // �ړ����͏���
    InputMove(elapsedTime);

    // �e�ۓ��͏���
    LuanchProjectile(elapsedTime);
}

void Unit_A::TransitionFallState()
{
    if (state == State::Damage || state == State::Death)
        return;

    state = State::Fall;

    model_unit_A->PlayAnimation(Anim_Falling, true);
}

void Unit_A::UpdateFallState(float elapsedTime)
{
    // �ړ����͏���
    InputMove(elapsedTime);


    // �e�ۓ��͏���
    LuanchProjectile(elapsedTime);
}

void Unit_A::TransitionLandState()
{
    // �_���[�W�A���S�X�e�[�g���͑J�ڂ��Ȃ�
    if (state == State::Damage || state == State::Death)
        return;

    state = State::Land;

    // ���n�A�j���[�V�����Đ�
    model_unit_A->PlayAnimation(Anim_Landing, false);
}

void Unit_A::UpdateLandState(float elapsedTime)
{ 
    // �A�j���[�V�����Đ����I������璅�n��ԏI��
    if (!model_unit_A->IsPlayAnimation())
    {
        TransitionIdleState();
    }
}

void Unit_A::TransitionAttackState()
{
    state = State::Attack;

    model_unit_A->PlayAnimation(Anim_Attack, false);
}

void Unit_A::UpdateAttackState(float elapsedTime)
{
    // �A�j���[�V�����Đ����I�������U����ԏI��
    if (!model_unit_A->IsPlayAnimation())
    {
        TransitionIdleState();
    }

    // �C�ӂ̃A�j���[�V������Ԃł̂ݏՓ˔��菈��������

    float attackStart = 0.3f;
    float attackEnd = 0.4f;
    float animeTime = model_unit_A->GetCurrentAnimationSecond();

    attackCollisionFlag = (animeTime > attackStart && animeTime < attackEnd);

    if (attackCollisionFlag)
    {
        // ����m�[�h�ƃG�l�~�[�̏Փˏ���
        CollisionNodeVsEnemies("mixamorig:LeftHand", leftHandRadius);
    }
}

void Unit_A::TransitionDamageState()
{
    state = State::Damage;

    // �_���[�W�A�j���[�V�����Đ�
    model_unit_A->PlayAnimation(Anim_GetHit1, false);
}

void Unit_A::UpdateDamageState(float elapsedTime)
{
    // �A�j���[�V�������I�������ҋ@�X�e�[�g�֑J��
    if (!model_unit_A->IsPlayAnimation())
        TransitionIdleState();
}

void Unit_A::TransitionDeathState()
{
    state = State::Death;

    // ���S�A�j���[�V�����Đ�
    model_unit_A->PlayAnimation(Anim_Death, false);
}

void Unit_A::UpdateDeathState(float elapsedTime)
{
    if (!model_unit_A->IsPlayAnimation())
    {
        // �{�^�����������畜���X�e�[�g�ɐ؂�ւ�
        GamePad& game_pad = Input::Instance().GetGamePad();
        if (game_pad.GetButtonDown() & GamePad::BTN_A)
        {
            TransitionReviveState();
        }
    }
}

void Unit_A::TransitionReviveState()
{
    state = State::Revive;

    // �̗͑S��
    health = max_health;

    // �����A�j���[�V�����Đ�
    model_unit_A->PlayAnimation(Anim_Revive, false);
}

void Unit_A::UpdateReviveState(float elapsedTime)
{
    // �����A�j���[�V�������I�������ҋ@�X�e�[�g�֑J��
    if (!model_unit_A->IsPlayAnimation())
        TransitionIdleState();
}

void Unit_A::OnLanding()
{
    // ���n�����璅�n�X�e�[�g�ɐ؂�ւ�
    if (state != State::Damage || state != State::Death)
    {
        TransitionLandState();
        jumpCount = 0;
    }
}

void Unit_A::OnDamage()
{
    TransitionDamageState();
}

void Unit_A::OnDead()
{
    TransitionDeathState();
}

DirectX::XMFLOAT3 Unit_A::GetMoveVec() const
{
    return DirectX::XMFLOAT3();
}

bool Unit_A::InputMove(float elapsedTime)
{ 
    // �i�s�x�N�g���擾
    DirectX::XMFLOAT3 vec = GetMoveVec();
    Move(vec.x, vec.z, moveSpeed);
    Turn(elapsedTime, vec.x, vec.z, moveSpeed);

    // �i�s�x�N�g�����[���x�N�g���łȂ��ꍇ�͓��͂��ꂽ
    if (vec.x != 0.0f || vec.z != 0.0f)
        return true;

    return false;
}

void Unit_A::CollisionPlayerVsEnemies()
{
}

void Unit_A::CollisionNodeVsEnemies(const char* nodeName, float nodeRadius)
{
}

void Unit_A::LuanchProjectile(float elapsedTime)
{
}

bool Unit_A::InputProjectile()
{
    return false;
}

bool Unit_A::InputAttack()
{
    return false;
}
