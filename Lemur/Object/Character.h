#pragma once
#include "../Model/Model.h"
#include "../Model/FbxModelManager.h"
#include "../Graphics/Graphics.h"
#include "../Resource/ResourceManager.h"
#include "./Lemur/Input/Mouse.h"
#include <d3d11.h>
#include <wrl.h>
#include <directxmath.h>
#include <vector>
#include <unordered_map>
#include <string>
#include <fbxsdk.h>

class Character
{
public:
    Character() {};
    virtual ~Character() {}

    // �`��ݒ�
    void Render(float scale, ID3D11PixelShader* replaced_pixel_shader);
    void Render(float scale, ID3D11PixelShader** replaced_pixel_shader);

    virtual void DrawDebugGUI() {};
    virtual void DrawDebugPrimitive() {};

    //---------Getter--------------------------------------------------------------------------
    
    // ���f���̐��l
    const DirectX::XMFLOAT3& GetPosition() const { return position; }                       // �ʒu
    const DirectX::XMFLOAT3& GetAngle() const { return rotation; }                          // ��]
    const DirectX::XMFLOAT3& GetScale() const { return scale; }                             // �X�P�[��
    const float& GetScaleFactor() const { return scaleFactor; }                             // �X�P�[���t�@�N�^�[
    const float GetRadius() const { return radius; }                                        // ���a
    const float GetHeight() const { return height; }                                        // ����
    const DirectX::XMFLOAT4& GetColor()const { return material_color; }                     // �F
    const FbxModelManager* GetModel()const { return model.get(); }                          // ���f��

    // �X�e�[�^�X
    const int GetHealth() const { return health; }                                          // HP
    const int GetMaxHealth() const { return max_health; }                                   // MaxHP
    const float GetAttackCollisionRange()const { return attack_collision_range; }           // �U�������蔻��
    const bool IsGround() const { return isGround; }                                        // �ݒu����

    // �A�j���[�V����
    const bool GetEndAnimation()const { return end_animation; }                             // �A�j���[�V�����I���t���O
    const int GetFrameIndex()const { return frame_index; }                                  // �A�j���[�V�����t���[��
    const std::vector<Animation>* GetAnimation()const { return model->GetAnimation(); };    // �A�j���[�V�����f�[�^


    //---------Setter--------------------------------------------------------------------------
    // ���f���̐��l
    void SetPosition(const DirectX::XMFLOAT3& position) { this->position = position; }      // �ʒu
    void SetPosition(const float pos_x, const float pos_y, const float pos_z) { position.x = pos_x, position.y = pos_y, position.z = pos_z; }
    void SetAngle(const DirectX::XMFLOAT3& rotation) { this->rotation = rotation; }         // ��]
    void SetScale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }                  // �X�P�[��
    void SetScaleFactor(const float scaleFactor) { this->scaleFactor = scaleFactor; }       // �X�P�[���t�@�N�^�[
    void SetModel(std::shared_ptr<FbxModelManager> Model) { this->model = Model; }          // ���f��
    void SetPixelShader(ID3D11PixelShader* ps) { pixelShader = ps; }                        // �s�N�Z���V�F�[�_�[    
    void SetAnimCalcRate(const float calc_rate_) { anim_calc_rate = calc_rate_; }           // �A�j���[�V�����̍Đ����x�{��


    // �_���[�W��^����
    bool ApplyDamage(int damage);

protected:
    // �A�j���[�V�����̐؂�ւ�
    void SetAnimationIndex(int index, const bool& loop = false)
    {
        frame_index = 0;
        animation_tick = 0;
        animation_index = index;
        end_animation = false;
        animation_loop_flag = loop;

        animation_blend_time = 0.0f;
        animation_blend_seconds = 1.0f;
    } 
   
    // �ړ�����
    void Move(float vx, float vz, float speed);
    // �����]��
    void Turn(float elapsedTime, float vx, float vz, float speed);

    // ���͍X�V
    void UpdateVelocity(float elapsedTime);

    // �q�b�g�X�g�b�v����(�b)
    void HitStopON(float hit_stop_time_);

    // �W�����v����
    void Jump(float speed);


    // ���n�������ɌĂ΂��
    virtual void OnLanding() {};

    // �_���[�W���󂯂��Ƃ��ɌĂ΂��
    virtual void OnDamage() {}

    // ���S�������ɌĂ΂��
    virtual void OnDead() {}

public:
    void UpdateScale() {
        scale.x = scale.y = scale.z = scaleFactor;
    }

    // �������͍X�V����
    void UpdataVerticalVelocity(float elapsedFrame);

    // �����ړ��X�V����
    void UpdateVerticalMove(float elapsedTime);

    // �������͍X�V����
    void UpdataHorizontalVelocity(float elapsedFrame);

    // �����ړ��X�V����
    void UpdateHorizontalMove(float elapsedTime);

    // �q�b�g�X�g�b�v�̌v�Z
    void HitStopCalc();

    // �s��̍X�V����
    void UpdateTransform();

    // ���G���ԍX�V
    void UpdateInvincibleTimer(float elapsedTime);

public:
    Animation::keyframe keyframe{};
    /* �U���AHP�֌W�̓X�L���Ƃ��M�����u���Ƃ��ŕϓ�����@�������
     * �Ë��Ńp�u���b�N�ȁB*/
    int     max_health = 5; // �ő匒�N���
    int     health = max_health; // ���N���
    bool    death = false;

    //��b�U����
    int attack_power = 0;
    // ���[�V�����l(���{)
    float motion_value = 1.0f;

    // �U���Ԋu
    float attack_interval = 0.0f;

    // ��b�h���
    float defense_power = 0;

    // �X�s�[�h�p�����[�^
    float speed_power = 10.0f;

    // �U�������蔻�肷�邩
    bool attack_collision_flag = false;

    bool animStop = false;

    Microsoft::WRL::ComPtr<ID3D11PixelShader> PS;

public:
    void UpdateAnimation(float elapsedTIme);
    void UpdateBlendRate(float blendRate, const float& elapsedTime);
    // �A�j���[�V�����Đ�����[17]
    bool IsPlayAnimation()const;
    // ���݂̃A�j���[�V�����ԍ�
    bool animation_loop_flag = true;
    float animation_blend_time = 0.0f;
    float animation_blend_seconds = 0.0f;

    DirectX::XMFLOAT4X4 World;

    DirectX::XMFLOAT3 velocity = { 0, 0, 0 };  // ���x
    DirectX::XMFLOAT3 position = { 0, 0, 0 };  // �ʒu
    DirectX::XMFLOAT3 scale = { 1.0f, 1.0f, 1.0f }; // �X�P�[��
    DirectX::XMFLOAT3 rotation = { 0, 0, 0 }; //�@��]
    DirectX::XMFLOAT4 material_color = { 1, 1, 1, 1 }; // �F

    DirectX::XMFLOAT3 direction = { 0,0,1 };// ����
    DirectX::XMFLOAT4X4 transform = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };// �s��

    float   invincibleTimer = 1.0f; // ���G����
    float   maxMoveSpeed = 5.0f; // �ő呬�x
    float   moveVecX = 0.0f; // X�����ړ�
    float   moveVecZ = 0.0f; // Z�����ړ�
    float   friction = 0.7f; // ���C��
    float   acceleration = 1.5f; // ������
    float   gravity = -1.0f; // �d��
    float   radius = 1.0f; // ���a
    float   height = 0.0f; // ���� 

    float   scaleFactor = 1.0f;

    float	airControl = 0.3f; // ��C��R

    bool    isGround = false; // ���n�t���O

    // �U�������蔻�肷�锼�a
    float attack_collision_range = 0.3f;

    // �����̑���(�X�s�[�h�p�����[�^�����邩��x�[�X�̑��x�͌�����)
    float walk_speed = 0.1f;


    float animation_tick = 0; // �A�j���[�V����
    std::shared_ptr<FbxModelManager> model;

    // �A�j���[�V�����p(�����o�����Ƃ��܂����B����ɑ�������ϐ��Ƃ������������炲�߂�)
    //���O��clip_index����ς��܂����B
    int animation_index = 0;
    int frame_index = 0;

    // ���[�g���[�V�����Ŏg��������܂����B by:tomy
    int root_motion_node_index = -1;

    // �A�j���[�V�����I���t���O
    bool end_animation = false;


    // �q�b�g�X�g�b�v���鎞��
    float hit_stop_time;
    // �q�b�g�X�g�b�v�o�ߎ���
    float hit_stop_timer;
    // �q�b�g�X�g�b�v���Ă邩
    bool is_hit_stop = false;

    // ���̒l��������΃q�b�g�X�g�b�v�ł���
    float hit_stop_rate = 1.0f;
    // �A�j���[�V�����̍Đ����x�𐧌䂵�Ă��낢��ȕ\��������z
    float anim_calc_rate = 1.0f;


    Mouse* mouse;


    ID3D11PixelShader* pixelShader = nullptr; // �V�F�[�_�[
};