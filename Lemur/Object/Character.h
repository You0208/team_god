#pragma once
#include "../Model/Model.h"
#include "../Graphics/Graphics.h"
#include "../Model/FbxModelManager.h"
#include "../Model/GltfModelManager.h"
#include "../Resource/ResourceManager.h"

class Character
{
public:
    Character() {};
    virtual ~Character() {}

    // �`��ݒ�
    void Render(float scale, ID3D11PixelShader* replaced_pixel_shader);
    void Render(float scale, ID3D11PixelShader** replaced_pixel_shader);

    // Imgui
    void DrawDebugGUI(std::string name,int i);
    virtual void DrawDebugGUI() {}
    // �f�o�b�O�v���~�e�B�u�`��
    virtual void DrawDebugPrimitive() {};
    // �_���[�W��^����
    bool ApplyDamage(int damage);

    // �s��̍X�V����
    void UpdateTransform();

protected:
    //----------���f���֘A------------------------------------------------
    // �T�C�Y�X�V
    void UpdateScale() {
        scale.x = scale.y = scale.z = scaleFactor;
    }

    void LoadFBXModel(ID3D11Device* device, const char* fbx_filename, bool triangulate = true, float sampling_rate = 0)
    {
        model = std::make_unique<FbxModelManager>(device, fbx_filename, triangulate, sampling_rate);
        is_gltf = false;
    }
    void LoadFBXModel(ID3D11Device* device, const char* fbx_filename, std::vector<std::string>& animation_filenames, bool triangulate = true, float sampling_rate = 0)
    {
        model = std::make_unique<FbxModelManager>(device, fbx_filename, animation_filenames, triangulate, sampling_rate);
        is_gltf = false;
    }

    void LoadGltfModel(ID3D11Device* device, const std::string& filename, bool external_texture)
    {
        gltf_model = std::make_unique<GltfModelManager>(device, filename, external_texture);
        is_gltf = true;
    }
    //----------�ړ��֘A------------------------------------------------
    // ���͍X�V
    void UpdateVelocity(float elapsed_fime);

    // �������͍X�V����
    void UpdataVerticalVelocity(float elapsed_frame);

    // �����ړ��X�V����
    void UpdateVerticalMove(float elapsed_time);

    // �������͍X�V����
    void UpdataHorizontalVelocity(float elapsed_frame);

    // �����ړ��X�V����
    void UpdateHorizontalMove(float elapsed_time);

    //----------�Q�[���֘A------------------------------------------------
    // ���G���ԍX�V
    void UpdateInvincibleTimer(float elapsed_time);

    // �ړ�����
    void Move(float vx, float vz, float speed);

    // �����]��
    void Turn(float elapsed_time, float vx, float vz, float speed);

    // �W�����v����
    void Jump(float speed);

    // ���n�������ɌĂ΂��
    virtual void OnLanding() {};

    // �_���[�W���󂯂��Ƃ��ɌĂ΂��
    virtual void OnDamage() {}

    // ���S�������ɌĂ΂��
    virtual void OnDead() {}

    //----------�A�j���[�V�����֘A------------------------------------------------
    // 
    // �q�b�g�X�g�b�v�̌v�Z
    void HitStopCalc();

    // �q�b�g�X�g�b�v����(�b)
    void HitStopON(float hit_stop_time_);

    // �A�j���[�V�����̍X�V
    void UpdateAnimation(float elapsedTime)
    {
        if (is_gltf)gltf_model->UpdateAnimation(elapsedTime);
        else model->UpdateAnimation(elapsedTime);
    }

    // �A�j���[�V�����Đ�����
    bool IsPlayAnimation()const
    {
        if (is_gltf)return gltf_model->IsPlayAnimation();
        else return  model->IsPlayAnimation();
        return false;
    }

    void PlayAnimation(
        const int& index,
        const bool& loop,
        const float& speed = 1.0f,
        const float& blendSeconds = 0.05f)
    {
        if (is_gltf)gltf_model->PlayAnimation(index, loop, speed, blendSeconds);
        else model->PlayAnimation(index, loop, speed, blendSeconds);
    }

    const int GetCurrentAnimationIndex()&
    {
        if (is_gltf)return gltf_model->GetCurrentAnimationIndex();
        else return  model->GetCurrentAnimationIndex();
    }

public:
    //---------Getter--------------------------------------------------------------------------
    
    // ���f���̐��l
    const DirectX::XMFLOAT3& GetPosition() const { return position; }                         // �ʒu
    const DirectX::XMFLOAT3& GetAngle() const { return rotation; }                            // ��]
    const DirectX::XMFLOAT3& GetScale() const { return scale; }                               // �X�P�[��
    const float& GetScaleFactor() const { return scaleFactor; }                               // �X�P�[���t�@�N�^�[
    const float& GetRadius() const { return radius; }                                         // ���a
    const float& GetHeight() const { return height; }                                         // ����
    const DirectX::XMFLOAT4& GetColor()const { return material_color; }                       // �F
    const FbxModelManager*   GetModel()const { return model.get(); }                          // ���f��

    // �X�e�[�^�X
    const int&  GetHealth() const { return health; }                                          // HP
    const int&  GetMaxHealth() const { return max_health; }                                   // MaxHP
    const int&  GetAttackPower() const { return attack_power; }                                   // MaxHP
    const float& GetAttackCollisionRange()const { return attack_collision_range; }            // �U�������蔻��
    const bool& IsGround() const { return isGround; }                                         // �ݒu����
    const bool& IsDead() const { return death; }                                              // ���S����

    // �A�j���[�V����
    const bool& GetEndAnimation()const { return end_animation; }                              // �A�j���[�V�����I���t���O
    const int&  GetFrameIndex()const { return frame_index; }                                  // �A�j���[�V�����t���[��
    const std::vector<Animation>* GetAnimation()const { return model->GetAnimation(); };      // �A�j���[�V�����f�[�^

    //---------Setter--------------------------------------------------------------------------
     
    // ���f���̐��l
    void SetPosition(const DirectX::XMFLOAT3& position) { this->position = position; }              // �ʒu
    void SetPosition(const float pos_x, const float pos_y, const float pos_z) { position.x = pos_x, position.y = pos_y, position.z = pos_z; }
    void SetAngle(const DirectX::XMFLOAT3& rotation) { this->rotation = rotation; }                 // ��]
    void SetScale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }                          // �X�P�[��
    void SetScaleFactor(const float scaleFactor) { this->scaleFactor = scaleFactor; }               // �X�P�[���t�@�N�^�[
    void SetModel(std::shared_ptr<FbxModelManager> Model) { this->model = Model; }                  // ���f��
    void SetAnimCalcRate(const float calc_rate_) { anim_calc_rate = calc_rate_; }                   // �A�j���[�V�����̍Đ����x�{��

    // �X�e�[�^�X
    void SetDead(bool death_) { this->death = death_; }                                               // ���S
    void SetAttackPower(const int attack_power_) { this->attack_power = attack_power_; }              // �U����
    void SetAttackInterval(const int attack_interval_) { this->attack_interval = attack_interval_; }  // �U���Ԋu
    void SetHealth(const int health_) { this->health = health_; }                                     // HP
    void SetRadius(const float radius_) { this->radius = radius_; }                                   // ���a
    void SetAttackRadius(const float attack_radius_) { this->attack_collision_range = attack_radius_; } // ���a
    void SetSpeedPower(const float speed_power_) { this->speed_power = speed_power_; }                // ���x

private:
    std::shared_ptr<FbxModelManager> model = nullptr;                           // FBX���f��
    std::shared_ptr<GltfModelManager> gltf_model = nullptr;                     // Gltf���f��

protected:
    //----------���f���֘A------------------------------------------------
    Microsoft::WRL::ComPtr<ID3D11PixelShader> PS    = nullptr;                     // �s�N�Z���V�F�[�_�[
    Animation::keyframe keyframe                    = {};                          // �L�[�t���[��
    bool                is_gltf                     = false;                       // �g�p���郂�f����GLTF��

    DirectX::XMFLOAT3   velocity                    = { 0, 0, 0 };                 // ���x
    DirectX::XMFLOAT3   position                    = { 0, 0, 0 };                 // �ʒu
    DirectX::XMFLOAT3   scale                       = { 1.0f, 1.0f, 1.0f };        // �X�P�[��
    float               scaleFactor                 = 1.0f;                        // �X�P�[���̂܂Ƃ�
    DirectX::XMFLOAT3   rotation                    = { 0, 0, 0 };                 //�@��]
    DirectX::XMFLOAT4   material_color              = { 1, 1, 1, 1 };              // �F
    DirectX::XMFLOAT3   direction                   = { 0,0,1 };                   // ����

    //----------�Q�[���֘A------------------------------------------------
    float   radius                 = 1.0f;          // ���a
    float   height                 = 0.0f;          // ���� 

    int     max_health             = 5;             // �ő匒�N���
    int     health                 = max_health;    // ���N���
    bool    death                  = false;         // ���S����

    int     attack_power           = 0;             // ��b�U����
    float   attack_interval        = 0.0f;          // �U���Ԋu
    float   attack_collision_range = 0.3f;          // �U�����a
    float   speed_power            = 10.0f;         // �X�s�[�h�p�����[�^
    float   defense_power          = 0;             // ��b�h���

    float   maxMoveSpeed           = 5.0f;          // �ő呬�x
    float   moveVecX               = 0.0f;          // X�����ړ�
    float   moveVecZ               = 0.0f;          // Z�����ړ�
    float   friction               = 0.7f;          // ���C��
    float   acceleration           = 1.5f;          // ������
    float   gravity                = -1.0f;         // �d��

    float   invincibleTimer        = 1.0f;          // ���G����
    float	airControl             = 0.3f;          // ��C��R

    bool    isGround               = false;         // ���n�t���O

    //----------�A�j���[�V�����֘A------------------------------------------------
    bool    animation_loop_flag     = true;         // ���[�v�t���O
    bool    end_animation           = false;        // �A�j���[�V�����I���t���O
    float   animation_blend_time    = 0.0f;         // �u�����h
    float   animation_blend_seconds = 0.0f;         // �u�����h�b
    float   animation_tick          = 0;            // �A�j���[�V����
    int     animation_index         = 0;            // �A�j���[�V�����ԍ�
    int     frame_index             = 0;            // �A�j���[�V�����̃N���b�v�ԍ�
    float   anim_calc_rate          = 1.0f;         // �A�j���[�V�����̍Đ����x

    
    float   hit_stop_time           =0.0f;          // �q�b�g�X�g�b�v���鎞��
    float   hit_stop_rate           = 1.0f;         // �q�b�g�X�g�b�v�W��
    float   hit_stop_timer          =0.0f;          // �q�b�g�X�g�b�v�o�ߎ���
   
    bool    is_hit_stop             = false;        // �q�b�g�X�g�b�v���Ă邩

};