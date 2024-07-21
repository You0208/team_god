#include "Character.h"

#include "Lemur/Math/Easing.h"
#include "Lemur/Math/MathHelper.h"
#include "Lemur/Collision/Collision.h"


bool Character::ApplyDamage(int damage)
{
    // ���S���Ă���Ԃ͌��N��Ԃ�ύX���Ȃ�
    if (health <= 0)return false;

    // �_���[�W����
    health -= damage;

    // ���S�ʒm
    if (health <= 0)
    {
        health = 0;
        death = true;
        OnDead();
    }
    else
    {
        OnDamage();
    }

    // ���N��Ԃ��ύX�����ꍇ��true��Ԃ�
    return true;
}

void Character::DrawDebugGUI(std::string name, int i)
{
    std::string num = std::to_string(i);
    name = name + num;
    std::string T = std::string("Transform") + name;
    if (ImGui::TreeNode(T.c_str()))
    {
        std::string de = std::string("death") + name;
        ImGui::Checkbox(de.c_str(), &death);

        std::string hp = std::string("HP") + name;
        ImGui::DragInt(hp.c_str(), &health, 5.0f, -5.0f);

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

void Character::Render(float scale_, ID3D11PixelShader* replaced_pixel_shader)
{
    if (is_gltf)
    {
       gltf_model->Render(scale_* model_scale, replaced_pixel_shader);
    }
    else
    {
        model->Render(scale_* model_scale, replaced_pixel_shader);
    }
}

void Character::Render(float scale_, ID3D11PixelShader** replaced_pixel_shader)
{
    if (is_gltf)
    {
        gltf_model->Render(scale_* model_scale,replaced_pixel_shader);
    }
    else
    {
        model->Render(scale_* model_scale, replaced_pixel_shader);
    }
}

void Character::CollisionRender(float scale_, ID3D11PixelShader* replaced_pixel_shader)
{
    if (collision_model)collision_model->Render(scale_ * collision_scale_facter, replaced_pixel_shader);
}


void Character::Move(float vx, float vz, float speed)
{
    // �ړ������x�N�g����ݒ�
    moveVecX = vx;
    moveVecZ = vz;

    // �ő呬�x�ݒ�
    maxMoveSpeed = speed;
}

void Character::Turn(float elapsed_time,float vx, float vz, float speed)
{
    speed *= high_resolution_timer::Instance().time_interval();

    // �x�N�g���̑傫�����擾
    float Length = sqrtf(vx * vx + vz * vz);

    // �x�N�g���̑傫����0�Ȃ�(�[���x�N�g���Ȃ�)
    if (Length <= 0.01)
    {
        return;
    }

    // �i�s�x�N�g���̐��K��
    vx = vx / Length;
    vz = vz / Length;

    // ���g�̉�]�l����O���������߂�B
    float frontX = sinf(rotation.y);
    float frontZ = cosf(rotation.y);

    // ��]�p�����߂邽�߂ɁA2�̒P�ʃx�N�g���̓��ς��v�Z����
    float dot = (vx * frontX) + (vz * frontZ);

    // dot �� -1.0f �` 1.0f �ɂȂ�B�Ȃ̂� rot �� 0.0f �` 2.0f �ɂȂ�B
    float rot = 1.0f - dot;

    // ���ς��������Ȃ�����
    if (rot < speed) speed = rot; // ���̕�������ς���p�x������������

    // ���E������s�����߂�2�̒P�ʃx�N�g���̊O�ς��v�Z����
    float cross = (vx * frontZ) - (vz * frontX);

    // 2D�̊O�ϒl�����̏ꍇ�����̏ꍇ�ɂ���č��E���]���s����
    // ���E������s�����Ƃɂ���č��E��]��I������
    if (cross < 0.0f)
    {
        rotation.y -= speed;
    }
    else
    {
        rotation.y += speed;
    }

}

void Character::Jump(float speed)
{
    // ������̗͂�ݒ�
    velocity.y = speed;
}

void Character::UpdateVelocity(float elapsed_time)
{
    // �o�߃t���[��
    float elapsed_frame = 60.0f * elapsed_time;

    if (!is_fly)
    {
        // �������͍X�V����
        UpdataVerticalVelocity(elapsed_frame);

        // �����ړ��X�V����
        UpdateVerticalMove(elapsed_time);

        // �������͍X�V����
        UpdataHorizontalVelocity(elapsed_frame);

        // �����ړ��X�V����
        UpdateHorizontalMove(elapsed_time);
    }
}

void Character::HitStopON(float hit_stop_time_)
{
    hit_stop_time = hit_stop_time_;
    hit_stop_timer = 0;
    is_hit_stop = true;
}

void Character::UpdataVerticalVelocity(float elapsedFrame)
{
    // �d�͏���
    velocity.y += gravity * elapsedFrame;//�O���r�e�B�ƒP�ʂ����킹�邽�߂�elapsedFrame���g���Ă���
}

void Character::UpdateVerticalMove(float elapsedTime)
{
    // �ړ�����
    position.y += velocity.y * elapsedTime;

    // �n�ʔ���
    if (position.y < 0.0f)
    {
        position.y = 0.0f;
        velocity.y = 0.0f;

        // ���n����
        if (!isGround)
        {
            OnLanding();
        }
        isGround = true;
    }
    else
    {
        isGround = false;
    }
    position.y = 0.0f;
}

void Character::UpdataHorizontalVelocity(float elapsedFrame)
{   // XZ���ʂ̑��͂���������
    float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
    if (length > 0.0f)
    {
        // ���C��
        float friction = this->friction * elapsedFrame;

        // �󒆂ɂ���Ƃ��͖��C�͂����炷
        if (!isGround)friction *= airControl;

        // ���C�ɂ�鉡�����̌�������
        if (length > friction)
        {
            float vx = velocity.x / length;
            float vz = velocity.z / length;

            velocity.x -= vx * friction;
            velocity.z -= vz * friction;
        }
        // �������̑��͂����C�͈ȉ��ɂȂ����̂ő��͂𖳌���
        else
        {
            velocity.x = 0;
            velocity.z = 0;
        }
    }

    // XZ���ʂ̑��͂���������
    if (length <= maxMoveSpeed)
    {
        // �ړ��x�N�g�����[���x�N�g���o�Ȃ��Ȃ��������
        float moveVecLength = sqrtf(moveVecX * moveVecX + moveVecZ * moveVecZ);
        if (moveVecLength > 0.0f)
        {
            // ������
            float acceleration = this->acceleration * elapsedFrame;

            // �󒆂ɂ���Ƃ��͖��C�͂����炷
            if (!isGround)acceleration *= airControl;

            // �ړ��x�N�g���ɂ���������
            velocity.x += moveVecX * acceleration;
            velocity.z += moveVecZ * acceleration;

            // �ő呬�x����
            float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
            if (length > maxMoveSpeed)
            {
                float vx = velocity.x / length;
                float vz = velocity.z / length;

                velocity.x = vx * maxMoveSpeed;
                velocity.z = vz * maxMoveSpeed;
            }
        }
    }
    // �ړ��x�N�g�������Z�b�g
    moveVecX = 0.0f;
    moveVecZ = 0.0f;
}

void Character::UpdateHorizontalMove(float elapsedTime)
{
    // �ړ�����
    position.x += velocity.x * elapsedTime;
    position.z += velocity.z * elapsedTime;
}

void Character::HitStopCalc()
{
    if (hit_stop_timer > hit_stop_time)
    {
        hit_stop_rate = 1.0f;
        is_hit_stop = false;

        return;
    }

    hit_stop_rate = Easing::InSine(hit_stop_timer, hit_stop_time, 1.0f, 0.0f);

    hit_stop_timer += high_resolution_timer::Instance().time_interval();
}

void Character::UpdateTransform()
{
    if (!is_gltf)
    {
        model->GetTransform()->SetPosition(position);
        model->GetTransform()->SetScale(scale);
        model->GetTransform()->SetRotation({ rotation.x,rotation.y,rotation.z,0 });

        if (collision_model)
        {
            collision_model->GetTransform()->SetPosition(collision_pos);
            collision_model->GetTransform()->SetScale(collision_scale);
            collision_model->GetTransform()->SetRotation({ collision_rotation.x,collision_rotation.y,collision_rotation.z,0 });
        }
    }
    else 
    {
        gltf_model->GetTransform()->SetPosition(position);
        gltf_model->GetTransform()->SetScale(scale);
        gltf_model->GetTransform()->SetRotation({ rotation.x,rotation.y,rotation.z,0 });

        if (collision_model)
        {
            collision_model->GetTransform()->SetPosition(collision_pos);
            collision_model->GetTransform()->SetScale(collision_scale);
            collision_model->GetTransform()->SetRotation({ collision_rotation.x,collision_rotation.y,collision_rotation.z,0 });
        }
    }
}

void Character::UpdateInvincibleTimer(float elapsedTime)
{
    if (invincibleTimer > 0.0f)
    {
        invincibleTimer -= elapsedTime;
    }
}
