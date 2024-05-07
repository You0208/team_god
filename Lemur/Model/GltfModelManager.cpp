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

    blend_current_animation_index = index;    // �Đ�����A�j���[�V�����ԍ���ݒ�
    //current_animation_seconds = 0.0f;     // �A�j���[�V�����Đ����ԃ��Z�b�g

    animation_loop_flag = loop;     // ���[�v�����邩
    animation_end_flag = false;    // �Đ��I���t���O�����Z�b�g

    animation_speed = speed;    // �A�j���[�V�����Đ����x

    animation_blend_time = 0.0f;
    animation_blend_seconds = blendSeconds;
    blend_animated_nodes = gltf_model->nodes;
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

    // �Đ����łȂ��Ȃ珈�����Ȃ�
    if (!IsPlayAnimation()) return;

    //// �u�����h���̌v�Z
    //float blendRate = 1.0f;
    //if (animation_blend_seconds > 0.0f)
    //{
    //    //blendRate = 0.0f;
    //    blendRate = animation_blend_time / animation_blend_seconds;
    //    blendRate *= blendRate;
    //    animation_blend_time += elapsed_time;
    //}
    //if (GetAnimation()->size() > 0)
    //{
    //    // ���݂̃A�j���[�V�����擾
    //    GltfModel::animation& animation = GetAnimation()->at(current_animation_index);
    //    // �A�j���[�V�����̊e�`�����l���ɑ΂��ď������s��
    //    for (vector<GltfModel::animation::channel>::const_reference channel : animation.channels)
    //    {
    //        // �`�����l���ɑΉ�����T���v���[���擾
    //        const GltfModel::animation::sampler& sampler{ animation.samplers.at(channel.sampler) };
    //        // �T���v���[�̓��͂ƂȂ�^�C�����C�����擾
    //        const vector<float>& timeline{ animation.timelines.at(sampler.input) };
    //        // �^�C�����C���̃T�C�Y��0�̏ꍇ�̓X�L�b�v�i�s���ȃf�[�^�̉\�������邽�߁j
    //        if (timeline.size() == 0)
    //        {
    //            continue;
    //        }
    //        // ��̃A�j���[�V�����ɉ��̃L�[�t���[�������邩
    //        int keyCount = static_cast<int>(timeline.size());
    //        for (int keyIndex = 0; keyIndex < keyCount - 1; ++keyIndex)
    //        {
    //            //���݂̃L�[�t���[��
    //            const float& keyframe0 = timeline.at(keyIndex);
    //            //���̃L�[�t���[��
    //            const float& keyframe1 = timeline.at(keyIndex + 1);
    //            if (current_animation_seconds >= keyframe0 &&
    //                current_animation_seconds < keyframe1)
    //            {
    //                // �Đ����ԂƃL�[�t���[���̎��Ԃ���⊮�����Z�o����
    //                //��̃L�[�t���[���̃t���[�����̊Ԃ̑傫��
    //                float fream_length = keyframe1 - keyframe0;
    //                //���݂����݂̃L�[�t���[���̃t���[��������ǂꂾ���i�񂾂�
    //                float elapsedFrame = (current_animation_seconds - keyframe0);
    //                // �⊮��
    //                float rate = elapsedFrame / fream_length;
    //                int nodeCount = static_cast<int>(gltf_model->nodes.size());
    //                // �L�[�t���[���̂��ׂẴm�[�h�̒l��⊮���ĕύX����
    //                for (int nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
    //                {
    //                    // ���݂̃��f���p���������Ă�
    //                    GltfModel::node& node = gltf_model->nodes[nodeIndex];
    //                    // �u�����h�⊮����
    //                    if (blendRate < 1.0f)
    //                    {
    //                        // ���̃L�[�t���[���̃f�[�^������
    //                        const ModelResource::NodeKeyData& key1 = keyframe1.nodeKeys.at(nodeIndex);
    //                        // �ʒu�⊮
    //                        DirectX::XMVECTOR key0_pos = DirectX::XMLoadFloat3(&node.translate);
    //                        DirectX::XMVECTOR key1_pos = DirectX::XMLoadFloat3(&key1.translate);
    //                        DirectX::XMVECTOR Position = DirectX::XMVectorLerp(key0_pos, key1_pos, blendRate);
    //                        // ��]�⊮
    //                        DirectX::XMVECTOR key0_rotate = DirectX::XMLoadFloat4(&node.rotate);
    //                        DirectX::XMVECTOR key1_rotate = DirectX::XMLoadFloat4(&key1.rotate);
    //                        DirectX::XMVECTOR Rotation = DirectX::XMQuaternionSlerp(key0_rotate, key1_rotate, blendRate);
    //                        // ���
    //                        DirectX::XMStoreFloat3(&node.translate, Position);
    //                        DirectX::XMStoreFloat4(&node.rotate, Rotation);
    //                    }
    //                    else
    //                    {
    //                    }
    //                }
    //            }
    //        }
    //    }
    //}
    //// �^�C�����C�����擾

    // �ŏI�t���[������
    if (animation_end_flag)
    {
        animation_end_flag = false; // �I���t���O�����Z�b�g
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
        animated_nodes = gltf_model->nodes;
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

    const int animationIndexEnd = static_cast<int>(gltf_model->animations.size());
    if (current_animation_index >= animationIndexEnd)
    {
        return false;
    }
    return true;
}
