#include "FbxModelManager.h"

// TODO: ���\�[�X�}�l�[�W���[�̎g�p
#define USE_RESOURCE_MANAGER

FbxModelManager::FbxModelManager(ID3D11Device* device, const char* fbx_filename, bool triangulate, float sampling_rate)
{
#ifdef USE_RESOURCE_MANAGER // ���\�[�X�}�l�[�W���[����
    fbx_model = ResourceManager::Instance().LoadModelResource(
        device,
        fbx_filename,
        triangulate,
        sampling_rate
    );
#else // ���\�[�X�}�l�[�W���[�Ȃ�
    fbx_model = std::make_unique<skinned_mesh>(device, fbx_filename, triangulate, sampling_rate);
#endif

}

FbxModelManager::FbxModelManager(ID3D11Device* device, const char* fbx_filename, std::vector<std::string>& animation_filenames, bool triangulate, float sampling_rate)
{
    fbx_model = ResourceManager::Instance().LoadModelResource(
        device, fbx_filename,
        animation_filenames,
        triangulate,
        sampling_rate
    );
}

// �`��
void FbxModelManager::Render(const float& scale, ID3D11PixelShader* replaced_pixel_shader)
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();

    // world�s��X�V
    DirectX::XMFLOAT4X4 world = {};
    DirectX::XMStoreFloat4x4(&world, GetTransform()->CalcWorldMatrix(scale));

    // Model�`��
    if (&keyframe)
    {
        fbx_model->Render(graphics.GetDeviceContext(), world, GetModelColor(), &keyframe, replaced_pixel_shader);
    }
    else
    {
        fbx_model->Render(graphics.GetDeviceContext(), world, GetModelColor(), nullptr, replaced_pixel_shader);
    }
}

void FbxModelManager::Render(const float& scale, ID3D11PixelShader** replaced_pixel_shader)
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();

    // world�s��X�V
    DirectX::XMFLOAT4X4 world = {};
    DirectX::XMStoreFloat4x4(&world, GetTransform()->CalcWorldMatrix(scale));

    // Model�`��
    if (&keyframe)
    {
        fbx_model->Render(graphics.GetDeviceContext(), world, GetModelColor(), &keyframe, replaced_pixel_shader);
    }
    else
    {
        fbx_model->Render(graphics.GetDeviceContext(), world, GetModelColor(), nullptr, replaced_pixel_shader);
    }
}

void FbxModelManager::DrawDebug(std::string i)
{
    GetTransform()->DrawDebug(i);
}


void FbxModelManager::PlayAnimation(
    const int& index, const bool& loop,
    const float& speed, const float& blendSeconds)
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


void FbxModelManager::UpdateBlendRate(float blendRate, const float& elapsedTime)
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

void FbxModelManager::UpdateAnimation(const float& elapsedTime)
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
    // �A�j���[�V�����Đ����Ԍo��
    current_animation_seconds += elapsedTime;

    // �w��̃A�j���[�V�����f�[�^���擾
    Animation& animation = GetAnimation()->at(current_animation_index);

    // ���݂̎��Ԃ��ǂ̃L�[�t���[���̊Ԃɂ���̂����肷��
    const float  frameIndex_float = (current_animation_seconds * animation.sampling_rate) * animation_speed;
    const size_t frameIndex = static_cast<const size_t>(frameIndex_float);

    // �ŏI�t���[��
    const size_t frameEnd = (animation.sequence.size() - 1);

    // �Đ����Ԃ��I�[���Ԃ𒴂�����
    if (frameIndex > frameEnd)
    {
        // ���[�v�Đ�����ꍇ
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
    // �Đ����ԂƃL�[�t���[���̎��Ԃ�����ԗ����Z�o����
    else if ((keyframe.nodes.size() > 0) && frameIndex < frameEnd)
    {
        // �u�����h���̌v�Z
        float blendRate = 0.1f;
        UpdateBlendRate(blendRate, elapsedTime);

        // �L�[�t���[���擾
        const std::vector<Animation::keyframe>& keyframes = animation.sequence;

        // ���݂̎��Ԃ��ǂ̃L�[�t���[���̊Ԃɂ���̂����肷��
        const Animation::keyframe* keyframe_[2] = {
            &keyframe,
            &keyframes.at(frameIndex + 1)
        };

        // �Đ����ԂƃL�[�t���[���̎��Ԃ����ԗ����v�Z����
        fbx_model->BlendAnimations(keyframe_, blendRate, keyframe);

        // �g�����X�t�H�[���X�V
        fbx_model->UpdateAnimation(keyframe);
    }
    else
    {
        keyframe = animation.sequence.at(frameIndex);
    }
}

bool FbxModelManager::IsPlayAnimation() const
{
    if (current_animation_index < 0) return false;

    const int animationIndexEnd = static_cast<int>(fbx_model->animation_clips.size());
    if (current_animation_index >= animationIndexEnd) return false;

    return true;
}
