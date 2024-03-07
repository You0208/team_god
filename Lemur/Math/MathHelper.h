#pragma once
#include <cstdlib>
#include  <DirectXMath.h>

// 浮動小数算術
class Mathf
{
public:
    // 線形補完
    static float Lerp(float a,float b,float t)
    {
        return a * (1.0f - t) + (b * t);
    }

    // 指定範囲のランダム値を計算する
    static float RandomRange(float min, float max)
    {
        /* より細かく乱数をとりたいから小数点第一位まで出す。
         * そのために10かけて広い範囲でランダムしてから
         * 10割って基準の位に戻す。
        */

        int Max = (int)max*10;
        int Min = (int)min*10;
        float num = rand() % (Max - Min) + Min;
        return num /= 10.0f;
    }
};

//(Left Hand Side 左辺, Right Hand Side 右辺)
//inline ... ヘッダに関数の中身を書く、グローバル関数はinline必要
inline DirectX::XMFLOAT3 operator+(const DirectX::XMFLOAT3& lhs, const DirectX::XMFLOAT3& rhs)
{
    return { lhs.x + rhs.x,lhs.y + rhs.y,lhs.z + rhs.z };
}

inline DirectX::XMFLOAT3 operator-(const DirectX::XMFLOAT3& lhs, const DirectX::XMFLOAT3& rhs)
{
    return { lhs.x - rhs.x,lhs.y - rhs.y,lhs.z - rhs.z };
}

inline DirectX::XMFLOAT3 operator*(const DirectX::XMFLOAT3& lhs, const DirectX::XMFLOAT3& rhs)
{
    return { lhs.x * rhs.x,lhs.y * rhs.y,lhs.z * rhs.z };
}

inline DirectX::XMFLOAT3 operator/(const DirectX::XMFLOAT3& lhs, const DirectX::XMFLOAT3& rhs)
{
    return { lhs.x / rhs.x,lhs.y / rhs.y,lhs.z / rhs.z };
}

inline DirectX::XMFLOAT3 operator*(const DirectX::XMFLOAT3& lhs, const float rhs)
{
    return { lhs.x * rhs,lhs.y * rhs,lhs.z * rhs };
}

inline DirectX::XMFLOAT3 operator/(const DirectX::XMFLOAT3& lhs, const float rhs)
{
    return { lhs.x / rhs,lhs.y / rhs,lhs.z / rhs };
}

inline DirectX::XMFLOAT3 operator*(float lhs, const DirectX::XMFLOAT3& rhs)
{
    return { lhs * rhs.x,lhs * rhs.y,lhs * rhs.z };
}

inline DirectX::XMFLOAT3 operator/(float lhs, const DirectX::XMFLOAT3& rhs)
{
    return { lhs / rhs.x,lhs / rhs.y,lhs / rhs.z };
}


inline DirectX::XMFLOAT3& operator+=(DirectX::XMFLOAT3& lhs, const DirectX::XMFLOAT3& rhs)
{
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    lhs.z += rhs.z;
    return lhs;
}

inline DirectX::XMFLOAT3& operator-=(DirectX::XMFLOAT3& lhs, const DirectX::XMFLOAT3& rhs)
{
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;
    lhs.z -= rhs.z;
    return lhs;
}

inline DirectX::XMFLOAT3& operator*=(DirectX::XMFLOAT3& lhs, const DirectX::XMFLOAT3& rhs)
{
    lhs.x *= rhs.x;
    lhs.y *= rhs.y;
    lhs.z *= rhs.z;
    return lhs;
}

inline DirectX::XMFLOAT3& operator/=(DirectX::XMFLOAT3& lhs, const DirectX::XMFLOAT3& rhs)
{
    lhs.x /= rhs.x;
    lhs.y /= rhs.y;
    lhs.z /= rhs.z;
    return lhs;
}

inline DirectX::XMFLOAT3& operator*=(DirectX::XMFLOAT3& lhs, const float rhs)
{
    lhs.x *= rhs;
    lhs.y *= rhs;
    lhs.z *= rhs;
    return lhs;
}

inline DirectX::XMFLOAT3& operator/=(DirectX::XMFLOAT3& lhs, const float rhs)
{
    lhs.x /= rhs;
    lhs.y /= rhs;
    lhs.z /= rhs;
    return lhs;
}

// 二間のベクトルの長さを計算
inline float Length(const DirectX::XMFLOAT3& f1, const DirectX::XMFLOAT3& f2)
{
    float ret{};
    DirectX::XMStoreFloat(
        &ret,
        DirectX::XMVector3Length(
            DirectX::XMVectorSubtract(
                DirectX::XMLoadFloat3(&f1),DirectX::XMLoadFloat3(&f2)
            )
        )
    );
    return ret;
}

// 二つのベクトルの内積を計算して角度thetaより大きかったらtrue
inline bool CalcAngle(DirectX::XMVECTOR Vec1, DirectX::XMVECTOR Vec2, float theta)
{
    Vec1 = DirectX::XMVector3Normalize(Vec1);
    Vec2 = DirectX::XMVector3Normalize(Vec2);
    float dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(Vec1, Vec2));
    if (dot > theta)
    {
        return true;
    }
    return false;
}

//正規化
inline DirectX::XMFLOAT3 Normalize(const DirectX::XMFLOAT3& f)
{
    DirectX::XMFLOAT3 ret{};
    DirectX::XMStoreFloat3(&ret, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&f)));
    return ret;
}
