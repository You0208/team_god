#include "GltfModelManager.h"

GltfModelManager::GltfModelManager(ID3D11Device* device, const std::string& filename, bool external_texture)
{
    gltf_model = std::make_unique<GltfModel>(device, filename, external_texture);
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

void GltfModelManager::PlayAnimation(const int& index, const bool& loop, const float& speed, const float& blendSeconds)
{
    // �ݒ�p�̃A�j���[�V�����ԍ������݂̃A�j���[�V�����ԍ��Ɠ����ꍇ��return
    if (current_animation_index == index) return;

    if (current_animation_index != -1)
    {
        animation_blend_seconds = blendSeconds;
        blend_animated_nodes = gltf_model->nodes;
    }
    else
    {
        current_animation_index = index;
        current_animation_seconds = 0.0f;     // �A�j���[�V�����Đ����ԃ��Z�b�g
    }

    blend_current_animation_index = index;    // �Đ�����A�j���[�V�����ԍ���ݒ�

    animation_loop_flag = loop;     // ���[�v�����邩
    animation_end_flag = false;    // �Đ��I���t���O�����Z�b�g
    animation_end_flag = false;

    animation_speed = speed;    // �A�j���[�V�����Đ����x

    animation_blend_time = 0.0f;
}

void GltfModelManager::UpdateBlendRate(float blendRate, const float& elapsedTime)
{
    if (animation_blend_time < animation_blend_seconds)
    {
        animation_blend_time += elapsedTime;
        if (animation_blend_time >= animation_blend_seconds)
        {
            animation_blend_time = animation_blend_seconds;
        }
        blendRate = animation_blend_time / animation_blend_seconds;
        blendRate *= blendRate;
    }
}

#if 1
void GltfModelManager::UpdateAnimation(const float& elapsed_time)
{
    using namespace std;
    using namespace DirectX;

    // �ŏI�t���[������
    if (gltf_model->end_flag)
    {
        animation_end_flag = false;
        gltf_model->end_flag = false; // �I���t���O�����Z�b�g
        current_animation_index = -1;    // �A�j���[�V�����ԍ����Z�b�g
        return;
    }
    if (current_animation_index < 0)return;

    // �u�����h���̌v�Z
    blend_rate = 1.0f;

    if (animation_blend_seconds > 0.0f)
    {
        //blendRate = 0.0f;
        blend_rate = animation_blend_time / animation_blend_seconds;
        blend_rate *= blend_rate;
        animation_blend_time += elapsed_time; 
    }

    if (blend_rate < 1.0f)
    {
        gltf_model->Animate(blend_current_animation_index, 0, blend_animated_nodes, blend_animation_loop_flag);
        //gltf_model->Animate(blend_current_animation_index, 0.0f, blend_animated_nodes, blend_animation_loop_flag);
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
        if (gltf_model->nodes.size() != 0)animated_nodes = gltf_model->nodes;
        // ��x�̂ݏ�����
        if (once)
        {
            current_animation_index = blend_current_animation_index;
            current_animation_seconds = 0;
            once = false;
        }

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
#else
void GltfModelManager::UpdateAnimation(const float& elapsed_time)
{
 
    if (current_animation_index < 0)return;

    animated_nodes = gltf_model->nodes;
    gltf_model->Animate(current_animation_index, time += elapsed_time, animated_nodes, animation_loop_flag);
}
#endif

bool GltfModelManager::IsPlayAnimation() const
{
    if (current_animation_index < 0) return false;
}
