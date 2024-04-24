#pragma once

#include <windows.h>
#include <tchar.h>
#include <sstream>
#include <d3d11.h>
#include <DirectXMath.h>
#include "misc.h"
#include <wrl.h>

class Camera
{
private: // シングルトン化
    Camera() {}
    ~Camera() {}

public:
    // 唯一のインスタンス取得
    static Camera& Instance()
    {
        static Camera camera;
        return camera;
    }

    // パースペクティブ設定
    void SetPerspectiveFov(Microsoft::WRL::ComPtr<ID3D11DeviceContext> dc);

    // 指定方向を向く
    void SetLookAt(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& focus, const DirectX::XMFLOAT3& up);
    void SetTarget(DirectX::XMFLOAT3   target_) { target = target_; };
    void SetRange(float   range_) { range = range_; };

    void Update(float elapsedTime);

    void DrawDebug();

private:
    void UpdateDebug();

    // ロックオン切り替え
    void InputLockOn();

    // ロックオン時のカメラの挙動
    void LockOnUpdate(float elapsedTime);

    // ロックオンしてない時のカメラの挙動
    void NonLockOnUpdate(float elapsedTime);

    
public:
    DirectX::XMMATRIX GetViewMatrix() { return V; }         // view行列取得
    DirectX::XMMATRIX GetProjectionMatrix() { return P; }   // projection行列取得
    DirectX::XMFLOAT4X4 GetWorld() { return world; }   // projection行列取得
    DirectX::XMVECTOR GetEye() { return Eye; }   // projection行列取得
    DirectX::XMVECTOR GetFocus() { return Focus; }

    DirectX::XMFLOAT3 GetAngle() { return angle; }

private:
    // view行列
    DirectX::XMMATRIX V = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
    // projection行列
    DirectX::XMMATRIX P = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };


    DirectX::XMVECTOR Eye{};
    DirectX::XMVECTOR Focus{};
    DirectX::XMVECTOR Up{};

    DirectX::XMFLOAT3 up{};
    DirectX::XMFLOAT3 front{};
    DirectX::XMFLOAT3 right{};

    DirectX::XMFLOAT3 eye = {};

    // 水平に見るんやなくてちょっと上から俯瞰するための補正
    float eye_y_offset = 8.0f;

    // 見たい人の足元みるんやなくてチョイ上
    float target_y_offset = 1.5f;

    DirectX::XMFLOAT4X4 world;
    // デバッグ用
    DirectX::XMFLOAT3   target = { 0,0,0 };
    DirectX::XMFLOAT3   angle = { 0,0,0 };
    float               rollSpeed = DirectX::XMConvertToRadians(180);
    float               range = 12.0f;

    float               maxAngleX = DirectX::XMConvertToRadians(45);
    float               minAngleX = DirectX::XMConvertToRadians(-45);

    // ロックオンしてるか
    bool is_lockOn = false;

public:/*--------------- 画面振動系 -------------*/

    //画面振動
    void ScreenVibrate(float volume/*振動量*/, float effectTime/*振動時間*/);
    // 画面振動の計算
    void ScreenVibrationUpdate();

private:
    //画面振動
    DirectX::XMFLOAT3 screenVibrationOffset{};//振動表現用の座標
    float vibrationVolume;//振動量
    float vibrationTime;//振動時間
    float vibrationTimer;//振動時間を測るためのタイマー

};
