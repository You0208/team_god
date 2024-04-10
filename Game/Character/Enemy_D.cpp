#include "Enemy_D.h"
#include "EnemyManager.h"

#include "Lemur/Collision/Collision.h"
#include "Lemur/Math/MathHelper.h"

Enemy_D::Enemy_D()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Enemy\\spider_v009.fbx");

    attack_power    = 1;        // �U����
    attack_interval = 3.0f;     // �U���Ԋu

    radius          = 1.0f;     // ���a
    height          = 1.0f;     //�f�o�b�O�p
    position.x      = 5.0f;     // �����ʒu
    rotation.y      = -90.0f;   // ��������

    move_state      = 0;        // �ړ����
    speed_power     = -1.0f;    // ���ړ���
    speed_power_Y   = -1.0f;    // �c�ړ���
    direction_state = 0;        // �����̏��
    dis_max         = 1.0f;     // �ړ��ʋK��l
    is_last_touched = false;    // �O���ɐG�ꂽ��

    health          = 10;       // HP

    // �Ƃ肠�����A�j���[�V����
    model->PlayAnimation(0, true);
}

void Enemy_D::Update(float elapsedTime)
{
    switch (state_index)
    {
    case StateIndex::Move_State:
        MoveUpdate(elapsedTime);
        break;
    case StateIndex::Attack_State:
        AttackUpdate(elapsedTime);
        break;
    };

    if (health <= 0)
    {
        OnDead();
    }

    // ���͏����X�V
    UpdateVelocity(elapsedTime);

    // ���I���ԍX�V
    UpdateInvincibleTimer(elapsedTime);

    // ���f���A�j���[�V�����X�V
    model->UpdateAnimation(elapsedTime);

    // Transform �̍X�V
    UpdateTransform();

    // �f�o�b�O
    DrawDebugGUI();
    DrawDebugPrimitive();
}

void Enemy_D::AttackUpdate(float elapsedTime)
{
    attack_timer += elapsedTime;

    if (attack_timer >= attack_interval)
    {
        if (Fence::Instance().ApplyDamage(attack_power))
        {
            model->PlayAnimation(1, false);
            attack_timer = 0.0f;
        }
    }
    if (!model->IsPlayAnimation())
    {
        model->PlayAnimation(7, true);
    }
}

void Enemy_D::MoveUpdate(float elapsedTime)
{
    switch (move_state)
    {
    case 0:
        // ���ړ�
        velocity.x = speed_power;
        // �ړ��ʂ��L�^
        dis += abs(speed_power) * elapsedTime;
        // �ړ��ʂ��K��l�𒴂�����
        if (dis >= dis_max)
        {
            dis = 0.0f;                             // �ړ��ʂ�������
            if (is_last_touched)direction_state = 0;// �O���ɐG��ċ�����ړ��������Œ�
            else direction_state = rand() % 2;      // ����̕���������
            is_last_touched = false;                // ������
            move_state = 1;                         // �ړ���Ԃ�ω�
        }

        // ����ɓ���������
        if (Collision::IntersectRectVsCircle(Fence::Instance().left_rect, { position.x,position.z }, radius))
        {
            velocity.x = 0.0f;                              // ���x���O��
            model->PlayAnimation(7, true);                  // �A�j���[�V������ҋ@��
            state_index = int(StateIndex::Attack_State);    // �X�e�[�g��ύX
        }

        break;
    case 1:
        // ���ړ�
        velocity.x = speed_power;
        // �ړ��ʂ��L�^
        dis += abs(speed_power) * elapsedTime;

        // �ړ��ʂ��K��l�𒴂�����or���ɍs����������
        if (dis >= dis_max|| is_last_touched)
        {
            dis = 0.0f;       // �ړ��ʂ�������
            velocity.z = 0.0f;// ���x��������
            move_state = 0;   // �ړ���Ԃ�ω�
        }

        // �c�ړ�
        switch (direction_state)
        {
        case 0:
            velocity.z = speed_power_Y;// �c�ړ�

            // �������ɐU��؂��Ă�����
            if (Collision::IntersectRectVsCircle(Fence::Instance().back_rect, { position.x,position.z }, radius))
            {
                is_last_touched = true;
            }
            else if (Collision::IntersectRectVsCircle(Fence::Instance().front_rect, { position.x,position.z }, radius))
            {
                velocity.x = velocity.z = 0.0f;                 // ���x���O��
                model->PlayAnimation(7, true);                  // �A�j���[�V������ҋ@��
                state_index = int(StateIndex::Attack_State);    // �X�e�[�g��ύX
            }

            break;
        case 1:
            velocity.z = -speed_power_Y;// �c�ړ�

            // �������ɐU��؂��Ă�����
            if (Collision::IntersectRectVsCircle(Fence::Instance().back_rect, { position.x,position.z }, radius))
            {
                is_last_touched = true;
            }
            else if (Collision::IntersectRectVsCircle(Fence::Instance().front_rect, { position.x,position.z }, radius))
            {
                velocity.x = velocity.z = 0.0f;                 // ���x���O��
                model->PlayAnimation(7, true);                  // �A�j���[�V������ҋ@��
                state_index = int(StateIndex::Attack_State);    // �X�e�[�g��ύX
            }
            break;
        }

        // ��ɓ���������
        if (Collision::IntersectRectVsCircle(Fence::Instance().left_rect, { position.x,position.z }, radius))
        {
            velocity.x = velocity.z = 0.0f;                 // ���x���O��
            model->PlayAnimation(7, true);                  // �A�j���[�V������ҋ@��
            state_index = int(StateIndex::Attack_State);    // �X�e�[�g��ύX
        }

        break;
    }
}

void Enemy_D::DrawDebugGUI()
{
    std::string name = "Enemy_B";
    std::string T = std::string("Transform") + name;
    if (ImGui::TreeNode(T.c_str()))
    {
        std::string spe = std::string("speed") + name;
        ImGui::DragFloat(spe.c_str(), &speed_power, 5.0f, -5.0f);

        std::string p = std::string("position") + name;
        ImGui::DragFloat3(p.c_str(), &position.x, 1.0f, -FLT_MAX, FLT_MAX);
        std::string s = std::string("scale") + name;
        ImGui::DragFloat3(s.c_str(), &scale.x, 0.001f, -FLT_MAX, FLT_MAX);

        std::string r = std::string("rotation") + name;
        DirectX::XMFLOAT3 rot{};
        rot.x = DirectX::XMConvertToDegrees(rotation.x);
        rot.y = DirectX::XMConvertToDegrees(rotation.y);
        rot.z = DirectX::XMConvertToDegrees(rotation.z);
        ImGui::DragFloat3(r.c_str(), &rot.x, 0.5f, -FLT_MAX, FLT_MAX);
        rotation.x = DirectX::XMConvertToRadians(rot.x);
        rotation.y = DirectX::XMConvertToRadians(rot.y);
        rotation.z = DirectX::XMConvertToRadians(rot.z);

        std::string s_f = std::string("scale_facter") + name;
        ImGui::DragFloat(s_f.c_str(), &scaleFactor, 0.001f, 0.001f, 1.0f);
        ImGui::TreePop();
    }
}

void Enemy_D::DrawDebugPrimitive()
{
    DebugRenderer* debug_renderer = Lemur::Graphics::Graphics::Instance().GetDebugRenderer();
    debug_renderer->DrawCylinder(position, radius, height, { 0,1,0,1 });
}

void Enemy_D::OnDead()
{
    EnemyManager::Instance().Remove(this);
}
