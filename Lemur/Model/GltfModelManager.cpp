#include "GltfModelManager.h"

GltfModelManager::GltfModelManager(ID3D11Device* device, const std::string& filename)
{
    gltf_model= std::make_unique<GltfModel>(device, filename);
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

    current_animation_index = index;    // �Đ�����A�j���[�V�����ԍ���ݒ�
    current_animation_seconds = 0.0f;     // �A�j���[�V�����Đ����ԃ��Z�b�g

    animation_loop_flag = loop;     // ���[�v�����邩
    animation_end_flag = false;    // �Đ��I���t���O�����Z�b�g

    animation_speed = speed;    // �A�j���[�V�����Đ����x

    animation_blend_time = 0.0f;
    animation_blend_seconds = blendSeconds;
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
    // �Đ����łȂ��Ȃ珈�����Ȃ�
    if (!IsPlayAnimation()) return;

    // �ŏI�t���[������
    if (animation_end_flag)
    {
        animation_end_flag = false; // �I���t���O�����Z�b�g
        current_animation_index = -1;    // �A�j���[�V�����ԍ����Z�b�g
        return;
    }
    if (current_animation_index < 0)return;


    // �A�j���[�V�����Đ����Ԍo��
    current_animation_seconds += elapsed_time;

    GltfModel::animation animation = GetAnimation()->at(current_animation_index);

    animated_nodes = gltf_model->nodes;
    gltf_model->Animate(current_animation_index, current_animation_seconds, animated_nodes, animation_loop_flag);
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

    const int animationIndexEnd = static_cast<int>(gltf_model->animations.size());
    if (current_animation_index >= animationIndexEnd) return false;

    return true;
}
