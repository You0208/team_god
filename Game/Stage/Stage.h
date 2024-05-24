#pragma once
#include "Lemur/Collision/Collision.h"
#include "Lemur/Model/FbxModelManager.h"

// �X�e�[�W
class Stage
{
public:
    Stage() {}
    virtual ~Stage() {}

    // �X�V����
    virtual void Update(float elapsedTime) = 0;

    //�`�揈��
    //virtual void Render(float elapsedTime, ID3D11PixelShader** replaced_pixel_shader) = 0;
    virtual void Render(float elapsedTime, ID3D11PixelShader** replaced_pixel_shader, ID3D11PixelShader** replaced_pixel_shader2) {};

    virtual void DrawDebugGui() {}

    Rect                GetStageCollision() { return stage_collision; }
    DirectX::XMFLOAT3   GetStagePos()       { return position; }
    DirectX::XMFLOAT2   GetVariableStageWidth()     { return variable_stage_width; }

protected:
    Rect                                stage_collision         = {};           // �X�e�[�W�̓����蔻��p
    DirectX::XMFLOAT2                   stage_width             = {};           // �X�e�[�W�̕��i���j
    DirectX::XMFLOAT3                   position                = {};           // �X�e�[�W�̒��S
    DirectX::XMFLOAT3                   scale                   = {};           // �X�e�[�W�̃T�C�Y�i�����p�j
    float                               scale_facter            = 0.0f;         // �X�P�[���t�@�N�^�[
    DirectX::XMFLOAT2                   variable_stage_width    = {};           // �ς̃X�e�[�W��
};