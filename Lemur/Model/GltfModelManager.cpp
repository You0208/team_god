#include "GltfModelManager.h"

GltfModelManager::GltfModelManager(ID3D11Device* device, const std::string& filename, bool external_texture)
{
    gltf_model = std::make_unique<GltfModel>(device, filename, external_texture);
    animated_nodes = gltf_model->nodes;
    blend_animated_nodes = gltf_model->nodes;
}

void GltfModelManager::Render(const float& scale, ID3D11PixelShader* replaced_pixel_shader)
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    // world�s��X�V
    DirectX::XMFLOAT4X4 world = {};
    DirectX::XMStoreFloat4x4(&world, GetTransform()->CalcWorldMatrix(scale));
    gltf_model->Render(graphics.GetDeviceContext(), world, animated_nodes, replaced_pixel_shader);
}

void GltfModelManager::Render(const float& scale, ID3D11PixelShader** replaced_pixel_shader)
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();

    // world�s��X�V
    DirectX::XMFLOAT4X4 world = {};
    DirectX::XMStoreFloat4x4(&world, GetTransform()->CalcWorldMatrix(scale));
    gltf_model->Render(graphics.GetDeviceContext(), world, animated_nodes, replaced_pixel_shader);
}


void GltfModelManager::DrawDebug()
{
    //GetTransform()->DrawDebug();
}

void GltfModelManager::PlayAnimation(const int& index, const bool& loop, const bool& is_blend_, const float& speed, const float& blendSeconds)
{
    // �ݒ�p�̃A�j���[�V�����ԍ������݂̃A�j���[�V�����ԍ��Ɠ����ꍇ��return
    if (current_animation_index == index) return;
    is_blend = is_blend_;
    if (is_blend)
    {
        animation_blend_seconds = blendSeconds;
        animation_blend_time = 0.0f;
        blend_current_animation_index = index;    // �Đ�����A�j���[�V�����ԍ���ݒ�

    }
    else
    {
        current_animation_index = index;
        current_animation_seconds = 0.0f;     // �A�j���[�V�����Đ����ԃ��Z�b�g
    }

    animation_loop_flag = loop;     // ���[�v�����邩
    animation_end_flag = false;    // �Đ��I���t���O�����Z�b�g
    gltf_model->end_flag = false;

    animation_speed = speed;    // �A�j���[�V�����Đ����x

}

void GltfModelManager::UpdateAnimation(const float& elapsed_time)
{
    using namespace std;
    using namespace DirectX;

    // �Đ����łȂ��Ȃ珈�����Ȃ�
    if (!IsPlayAnimation()) return;

    // �ŏI�t���[������
    if (gltf_model->end_flag)
    {
        animation_end_flag = false;
        gltf_model->end_flag = false; // �I���t���O�����Z�b�g
        current_animation_index = -1;    // �A�j���[�V�����ԍ����Z�b�g
        return;
    }
    if (current_animation_index < 0)return;


    if (is_blend)
    {
        // �u�����h���̌v�Z
        blend_rate = 1.0f;
        if (animation_blend_seconds > 0.0f)
        {
            blend_rate = animation_blend_time / animation_blend_seconds;
            blend_rate *= blend_rate;
            animation_blend_time += elapsed_time;
        }

        // �u�����h����1.0�ȉ��̎�
        if (blend_rate < 1.0f)
        {
            gltf_model->Animate(blend_current_animation_index, 0, blend_animated_nodes, blend_animation_loop_flag);
            gltf_model->Animate(current_animation_index, current_animation_seconds, animated_nodes, animation_loop_flag);

            const std::vector<GltfModel::node>* nodes[2] = {
                 &animated_nodes,
                 &blend_animated_nodes
            };

            gltf_model->BlendAnimation(nodes, blend_rate, &animated_nodes);

            // �ʏ�A�j���[�V�����̏������t���O
            once = true;
        }
        else// �ʏ�A�j���[�V����
        {
            // ��x�̂ݏ�����
            if (once)
            {
                current_animation_index = blend_current_animation_index;
                current_animation_seconds = 0;
                once = false;
            }

            // �A�j���[�V�����Đ����Ԍo��
            current_animation_seconds += elapsed_time;

            // �ʏ�ʂ�A�j���[�V����
            gltf_model->Animate(current_animation_index, current_animation_seconds, animated_nodes, animation_loop_flag);
            if (gltf_model->end_flag)
            {
                if (animation_loop_flag)
                {
                    current_animation_seconds = 0.0f;
                    return;
                }
                // ���[�v�Đ����Ȃ��ꍇ
                else
                {
                    animation_end_flag = true;
                    return;
                }
            }

        }
    }
    else// �ʏ�A�j���[�V����
    {
        // �A�j���[�V�����Đ����Ԍo��
        current_animation_seconds += elapsed_time;

        gltf_model->Animate(current_animation_index, current_animation_seconds, animated_nodes, animation_loop_flag);
        if (gltf_model->end_flag)
        {
            if (animation_loop_flag)
            {
                current_animation_seconds = 0.0f;
                return;
            }
            // ���[�v�Đ����Ȃ��ꍇ
            else
            {
                animation_end_flag = true;
                return;
            }
        }

    }
}

bool GltfModelManager::IsPlayAnimation() const
{
    if (current_animation_index < 0) return false;

    return true;
}

void GltfModelManager::Dissolve(const float& elapsedTime)
{
    if (gltf_model->threshold >= 0)
    {
        gltf_model->threshold -= elapsedTime;
    }
    else  is_dissolve = false;
}
