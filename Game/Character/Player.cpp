#include "Player.h"
#include "Lemur/Input/Input.h"
static Player* instance = nullptr;

Player::Player()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources_2\\Model\\nico.fbx");

    scale.x = scale.y = scale.z = 1.0f;
    radius = 0.5f;
    height = 1.0f;
    position.z = -5.0f;

    // �Ƃ肠�����A�j���[�V����
    model->PlayAnimation(0, true);
}

Player::~Player()
{
}

void Player::Update(float elapsedTime)
{
    // ���͏���
    InputProcess(elapsedTime);

    // ���͏����X�V
    UpdateVelocity(elapsedTime);

    // ���I���ԍX�V
    UpdateInvincibleTimer(elapsedTime);

    // ���f���A�j���[�V�����X�V
    model->UpdateAnimation(elapsedTime);

    // �s��X�V����
    UpdateTransform();

    // Imgui
    DrawDebugGUI();
}
 
void Player::Render(float scale, ID3D11PixelShader** replaced_pixel_shader)
{
    model->DrawDebug(3);
    model->Render(scale, replaced_pixel_shader);
}

void Player::DrawDebugGUI()
{
    ImGui::Begin("Player");

    ImGui::SliderFloat("moveSpeed", &moveSpeed, 0.0f, 10.0f);
    
    ImGui::End();
}

Player& Player::Instance()
{
    return *instance;
}

void Player::InputProcess(float elapsed_time)
{
    GamePad& gamePad = Input::Instance().GetGamePad();

    speed = gamePad.GetAxisLX() * moveSpeed* elapsed_time;

    position.x += speed;
}
