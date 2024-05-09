#include "Camera.h"

#include <Lemur/Math/Easing.h>

#include"..\Input/Input.h"
#include "Lemur/Math/MathHelper.h"
#include "Lemur/Graphics/Graphics.h"
#include "imgui.h"

void Camera::SetLookAt(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& focus, const DirectX::XMFLOAT3& up)
{
    //視点、注視点、上方向からビュー行列を作成
    Eye = DirectX::XMLoadFloat3(&eye);
    Focus = DirectX::XMLoadFloat3(&focus);
    Up = DirectX::XMLoadFloat3(&up);
    DirectX::XMMATRIX View = DirectX::XMMatrixLookAtLH(Eye, Focus, Up);

    // ビュー行列を逆行列化し、ワールド行列に戻す
    DirectX::XMMATRIX World = DirectX::XMMatrixInverse(nullptr, View);
    DirectX::XMFLOAT4X4 world;
    DirectX::XMStoreFloat4x4(&world, World);

    // カメラ方向を取り出す
    this->right.x = world._11;
    this->right.y = world._12;
    this->right.z = world._13;

    this->up.x = world._21;
    this->up.y = world._22;
    this->up.z = world._23;

    this->front.x = world._31;
    this->front.y = world._32;
    this->front.z = world._33;
}

void Camera::SetPerspectiveFov(Microsoft::WRL::ComPtr<ID3D11DeviceContext> dc)
{
    D3D11_VIEWPORT viewport{};
    UINT num_viewports{ 1 };
    dc->RSGetViewports(&num_viewports, &viewport);

    float aspect_ratio{ viewport.Width / viewport.Height };
    // 射影行列を生成
    P = { DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(30), aspect_ratio, 0.1f, 1000.0f) };

    // 視線行列を生成
    //DirectX::XMVECTOR eye{ DirectX::XMLoadFloat4(&camera_position) };
    V = { DirectX::XMMatrixLookAtLH(Eye, Focus, Up) };
}

void Camera::Update(float elapsedTime)
{
    GamePad& gamePad = Input::Instance().GetGamePad();
    Mouse& mouse = Input::Instance().GetMouse();
    if (gamePad.GetButtonDown() & GamePad::BTN_LEFT_SHOULDER||
        gamePad.GetButtonDown() & GamePad::BTN_RIGHT_SHOULDER||
        mouse.GetButtonDown()&Mouse::BTN_MIDDLE)
    {
        InputLockOn();
    }

    ScreenVibrationUpdate();

    if (is_lockOn) LockOnUpdate(elapsedTime);

    else NonLockOnUpdate(elapsedTime);

    eye.x += screenVibrationOffset.x;
    eye.y += screenVibrationOffset.y;
    eye.z += screenVibrationOffset.z;

    target.x += screenVibrationOffset.x;
    target.y += screenVibrationOffset.y;
    target.z += screenVibrationOffset.z;

    //カメラの視点と注視点を設定
    SetLookAt(eye, target, DirectX::XMFLOAT3(0, 1, 0));
}

void Camera::DrawDebug()
{
    ImGui::Begin("Camera");

    DirectX::XMFLOAT3 angle_d = { DirectX::XMConvertToDegrees(angle.x),
        DirectX::XMConvertToDegrees(angle.y),
        DirectX::XMConvertToDegrees(angle.y)
    };
    ImGui::SliderFloat3(u8"角度", &angle_d.x, 0.0f, 360.0f);
    angle = { DirectX::XMConvertToRadians(angle_d.x),
        DirectX::XMConvertToRadians(angle_d.y),
        DirectX::XMConvertToRadians(angle_d.y)
    };

    ImGui::End();
}

void Camera::UpdateDebug()
{
}

void Camera::InputLockOn()
{
}

void Camera::LockOnUpdate(float elapsedTime)
{
}

void Camera::NonLockOnUpdate(float elapsedTime)
{
    GamePad& game_pad = Input::Instance().GetGamePad();

    /*----------- コントローラー処理 ------------*/
    float ax = game_pad.GetAxisRX();
    float ay = game_pad.GetAxisRY();

    float lx = game_pad.GetAxisLX();
    float ly = game_pad.GetAxisLY();

    float wx = game_pad.GetTriggerL();
    float wy = game_pad.GetTriggerR();
    // カメラの回転速度
    float speed = rollSpeed * elapsedTime;

    {
        // IMGUIのマウス入力値を使ってカメラ操作する
        ImGuiIO io = ImGui::GetIO();

        // マウスカーソルの移動量を求める
        float moveX = io.MouseDelta.x * 0.02f;
        float moveY = io.MouseDelta.y * 0.02f;

        // マウス左ボタン押下中
        if (io.MouseDown[ImGuiMouseButton_Right])
        {
            // Y軸回転
            angle.y += moveX * 0.5f;
            if (angle.y > DirectX::XM_PI)
            {
                angle.y -= DirectX::XM_2PI;
            }
            else if (angle.y < -DirectX::XM_PI)
            {
                angle.y += DirectX::XM_2PI;
            }
            // X軸回転
            angle.x += moveY * 0.5f;
            if (angle.x > DirectX::XM_PI)
            {
                angle.x -= DirectX::XM_2PI;
            }
            else if (angle.x < -DirectX::XM_PI)
            {
                angle.x += DirectX::XM_2PI;
            }
        }
    }

    //TODO Debug
    if (game_pad.GetButton() & game_pad.BTN_B)
    {
        angle.x -= wx * speed;
        angle.y += wy * speed;
    }
    else
    {
        angle.x += wx * speed;
        angle.y -= wy * speed;
    }

    //angle.x -= ax * speed;
    //angle.y += ay * speed;

#if 0
    /*------------- マウス処理 -------------*/
    Mouse& mouse = Input::Instance().GetMouse();

    //delta  pos
    float mouse_pos_x = static_cast<int> (mouse.GetPositionX() - mouse.GetOldPositionX());
    float mouse_pos_y = static_cast<int> (mouse.GetPositionY() - mouse.GetOldPositionY());

    // なんか9.0fずれてる
    mouse_pos_y -= 9.0f;
    //if (mouse_pos_x < 10.0f&& mouse_pos_x > -10.0f)mouse_pos_x = 0.0f;
    //if (mouse_pos_y < 5.0f&& mouse_pos_y > -5.0f)mouse_pos_y = 0.0f;

    // マウスでの感度の補正値、rollSpeedにかける。
    float speed_fit = 0.05f;

    // カメラの回転速度。
    speed = speed_fit * rollSpeed * elapsedTime;


    // スティックの入力値に合わせてX軸とY軸を回転。
    angle.x += mouse_pos_y * speed;
    angle.y += mouse_pos_x * speed;
# endif
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
#if 0

    SetCursorPos(1920 / 2, 1080 / 2);
    mouse.SetPositionX(1920 / 2);
    mouse.SetPositionY(1080 / 2);
#endif
    // カメラの回転値を回転行列に変換
    DirectX::XMMATRIX Transform = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);

    // 回転行列から前方向にベクトルを出す
    DirectX::XMVECTOR Front = Transform.r[2];
    DirectX::XMFLOAT3 front;
    DirectX::XMStoreFloat3(&front, Front);

    eye.x = target.x - (front.x * range);               // front=方向、長さを掛け算することで長さがわかる
    eye.y = target.y - (front.y * range) + eye_y_offset;// 目標地点からカメラの距離を引くことでカメラの位置が出る
    eye.z = target.z - (front.z * range);               // 解説８ページ
    // X軸のカメラ回転を制限
    angle.x = min(angle.x, maxAngleX);
    angle.x = max(angle.x, minAngleX);
    //模範解答↓
    if (angle.x < minAngleX)
    {
        angle.x = minAngleX;
    }
    if (angle.x > maxAngleX)
    {
        angle.x = maxAngleX;
    }

    // Y軸の回転値を-3.14~3.14に収まるようにする
    if (angle.y < -DirectX::XM_PI)
    {
        angle.y += DirectX::XM_2PI;
    }
    if (angle.y > -DirectX::XM_PI)
    {
        angle.y -= DirectX::XM_2PI;
    }

    ////カメラの視点と注視点を設定
    //SetLookAt(eye, target, DirectX::XMFLOAT3(0, 1, 0));

}

void Camera::ScreenVibrate(float volume, float effectTime)
{
    vibrationVolume = volume;
    vibrationTimer = effectTime;
    vibrationTime = effectTime;
}

void Camera::ScreenVibrationUpdate()
{
    screenVibrationOffset = {};
    if (vibrationTimer <= 0)return;

    //振動方向の指定(乱数)
    DirectX::XMFLOAT3 vibVec;
    DirectX::XMFLOAT3 right = this->right;
    DirectX::XMFLOAT3 up = this->up;

    right = right * Mathf::RandomRange(-50.0f, 50.0f);
    up = up * Mathf::RandomRange(-50.0f, 50.0f);

    vibVec = {
        right.x + up.x,
        right.y + up.y,
        0.0f
    };
    vibVec = Normalize(vibVec);

    //イージングを使い経過時間で振動量を調整する
    float vibrationVolume = Easing::InSine(vibrationTimer, vibrationTime, this->vibrationVolume, 0.0f);

    //振動値を入れる
    screenVibrationOffset = vibVec * vibrationVolume;

    vibrationTimer -= high_resolution_timer::Instance().time_interval();

}
