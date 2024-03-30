#include "Player.h"
static Player* instance = nullptr;

Player::Player()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    model_player = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources_2\\Model\\nico.fbx");

    scale.x = scale.y = scale.z = 0.01f;
    radius = 0.5f;
    height = 1.0f;

    // とりあえずアニメーション
    model_player->PlayAnimation(0, true);
}

Player::~Player()
{
}

void Player::Update(float elapsedTime)
{
    // 速力処理更新
    UpdateVelocity(elapsedTime);

    // 無的時間更新
    UpdateInvincibleTimer(elapsedTime);

    // モデルアニメーション更新
    model_player->UpdateAnimation(elapsedTime);
}

void Player::Render(float scale, ID3D11PixelShader** replaced_pixel_shader)
{
    model_player->Render(scale, replaced_pixel_shader);
}

void Player::DrawDebugGUI()
{
}

Player& Player::Instance()
{
    return *instance;
}
