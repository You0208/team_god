#pragma once
#include "./Lemur/Scene/BaseScene.h"

//Game
#include "../Stage/Stage.h"
#include "../Stage/Fence.h"
#include "../Character/Player.h"

class GameScene :public Lemur::Scene::BaseScene
{
public:
    GameScene() {}
    ~GameScene() override {}

    // ������
    void Initialize()override;

    // �I����
    void Finalize()override;

    // �X�V����
    void Update(HWND hwnd, float elapsedTime)override;

    // �`�揈��
    void Render(float elapsedTime)override;

    // Imgui
    void DebugImgui()override;

    // �����̏�����
    void InitializeLight()override;
private:// �Q�[��
    Stage* stage = nullptr;

    Player* player = nullptr;

    Fence* fence = nullptr;

    float   timer = 0.0f;
    float   time_limit;
    bool    time_up = false;

    // �R���g���[���[�p
    float s_l;
    float timer_s;
    float s_l_max;
    float f_d = 0;
    std::shared_ptr<Sprite> ohajiki;

    std::unique_ptr<GltfModelManager> gltf_test_model;
private:// �V�F�[�_�[
    Microsoft::WRL::ComPtr<ID3D11PixelShader> unit_ps;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> enemy_ps;

    // shader
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_views[8];

    // dissolve
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> noise;

    // skyMap
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> skymap;

    std::unique_ptr<StaticMeshBatch>  static_meshes[8];

    // �O�p�`
    Triangle Try_T;
    // �~
    DirectX::XMFLOAT2 P0;
    float r0;

    // �Ńo�O
    std::unique_ptr<FbxModelManager> test_model;
    std::unique_ptr<FbxModelManager> test_model_2;
    Effect* hitEffect;

    // �f�o�b�O
    Rect rect;
    DirectX::XMFLOAT2 c_p;
    float c_r;
    float angle;
    // �����A�j���[�V�����t���p�[�e�B�N���V�X�e��
    std::unique_ptr<ParticleSystem> particle_bomb;


};