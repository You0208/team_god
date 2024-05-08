#pragma once
#include "Lemur/Graphics/Graphics.h"
#include "Lemur/Resource/ResourceManager.h"
#include "Lemur/Model/Model.h"
#include "Lemur/Math/Transform.h"
class FbxModelManager
{
public:
    FbxModelManager(ID3D11Device* device, const char* fbx_filename, bool triangulate = true, float sampling_rate = 0);
    FbxModelManager(ID3D11Device* device, const char* fbx_filename, std::vector<std::string>& animation_filenames, bool triangulate = true, float sampling_rate = 0);
    ~FbxModelManager() {}

    void Render(const float& scale, ID3D11PixelShader* replaced_pixel_shader);
    void Render(const float& scale, ID3D11PixelShader** replaced_pixel_shader); 

    void DrawDebug(std::string i);

    Transform* GetTransform() { return &transform; }

    void SetModelColor(DirectX::XMFLOAT4 c) { color = c; }
    DirectX::XMFLOAT4 GetModelColor() { return color; }

public: // �擾�E�ݒ�֐��֘A
    // �A�j���[�V�����f�[�^�擾
    std::vector<Animation>* GetAnimation() { return &fbx_model->animation_clips; };
    const bool GetIsDissolve()& { return is_dissolve; }
    // ���݂̃A�j���[�V�����ԍ��̎擾�E�ݒ�
    const int GetCurrentAnimationIndex()& { return current_animation_index; }
    const int GetCurrentAnimationSecond()& { return current_animation_seconds; }
    void SetCurrentAnimationIndex(const int& animationIndex) { current_animation_index = animationIndex; }

    // �A�j���[�V�����Đ����x�ݒ�i�r���ōĐ����x��ς������Ƃ��ȂǂɁj
    void SetAnimationSpeed(const float& speed) { animation_speed = speed; }

public: // �A�j���[�V�����֐��֘A
    // �A�j���[�V�����Đ��ݒ�
    // (�A�j���[�V�����ԍ��E���[�v���邩�ǂ����E�A�j���[�V�����Đ����x�E�X���[�Y�؂�ւ����ԁi���x�j)
    void PlayAnimation(
        const int& index,
        const bool& loop,
        const float& speed = 1.0f,
        const float& blendSeconds = 1.0f
    );

    // �u�����h���̌v�Z�X�V����
    void UpdateBlendRate(float blendRate, const float& elapsedTime);

    // �A�j���[�V�����X�V����
    void UpdateAnimation(const float& elapsedTime);

    // �A�j���[�V�������Đ������ǂ���
    bool IsPlayAnimation() const;

    void Dissolve(const float& elapsedTime);
public: // �f�o�b�O�m�F�p
    bool is_blend_animation = true;     // �A�j���[�V�����u�����h�I���I�t

public:
    std::shared_ptr<SkinnedMesh> fbx_model;
    Transform    transform;

    Animation::keyframe keyframe = {};
    DirectX::XMFLOAT4 color = { 1,1,1,1 };

    float       animation_blend_time      = 0.0f;     // ���݂̃u�����h�^�C�}�[
private:
    float       animation_speed           = 1.0f;     // �A�j���[�V�����Đ����x
    float       current_animation_seconds = 0.0f;     // ���݂̃A�j���[�V�����Đ�����

    float       animation_blend_seconds   = 0.0f;     // �u�����h����

    int         current_animation_index   = -1;	      // ���݂̃A�j���[�V�����ԍ�

    bool        animation_loop_flag       = false;    // �A�j���[�V���������[�v�Đ����邩
    bool        animation_end_flag        = false;    // �A�j���[�V�������I��������

    bool        is_dissolve               = true;     // �����Ă�Œ���
};