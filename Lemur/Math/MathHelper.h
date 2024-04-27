#pragma once
#include <cstdlib>
#include  <DirectXMath.h>

struct Rect
{
    DirectX::XMFLOAT2 left_up;
    DirectX::XMFLOAT2 right_down;
    DirectX::XMFLOAT2 left_down = {};
    DirectX::XMFLOAT2 right_up = {};
    DirectX::XMFLOAT2 width = {};
    DirectX::XMFLOAT2 center = {};
};

struct Triangle
{
    DirectX::XMFLOAT2 A;
    DirectX::XMFLOAT2 B;
    DirectX::XMFLOAT2 C;
};

// ���������Z�p
class Mathf
{
public:
    // ���`�⊮
    static float Lerp(float a,float b,float t)
    {
        return a * (1.0f - t) + (b * t);
    }

    // �w��͈͂̃����_���l���v�Z����
    static float RandomRange(float min, float max)
    {
        /* ���ׂ����������Ƃ肽�����珬���_���ʂ܂ŏo���B
         * ���̂��߂�10�����čL���͈͂Ń����_�����Ă���
         * 10�����Ċ�̈ʂɖ߂��B
        */

        int Max = (int)max*10;
        int Min = (int)min*10;
        float num = rand() % (Max - Min) + Min;
        return num /= 10.0f;
    }
    static void NormalizeDirectXVector(DirectX::XMFLOAT4& vec) {
        DirectX::XMVECTOR vector = DirectX::XMLoadFloat4(&vec);
        vector = DirectX::XMVector4Normalize(vector);
        DirectX::XMStoreFloat4(&vec, vector);
    }

};


//(Left Hand Side ����, Right Hand Side �E��)
//inline ... �w�b�_�Ɋ֐��̒��g�������A�O���[�o���֐���inline�K�v
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

// 2D
inline DirectX::XMFLOAT2 operator+(const DirectX::XMFLOAT2& lhs, const DirectX::XMFLOAT2& rhs)
{
    return { lhs.x + rhs.x,lhs.y + rhs.y};
}
inline DirectX::XMFLOAT2 operator-(const DirectX::XMFLOAT2& lhs, const DirectX::XMFLOAT2& rhs)
{
    return { lhs.x - rhs.x,lhs.y - rhs.y};
}

inline DirectX::XMFLOAT2 operator*(const DirectX::XMFLOAT2& lhs, const DirectX::XMFLOAT2& rhs)
{
    return { lhs.x * rhs.x,lhs.y * rhs.y };
}

inline DirectX::XMFLOAT2 operator/(const DirectX::XMFLOAT2& lhs, const DirectX::XMFLOAT2& rhs)
{
    return { lhs.x / rhs.x,lhs.y / rhs.y };
}

inline DirectX::XMFLOAT2 operator*(const DirectX::XMFLOAT2& lhs, const float rhs)
{
    return { lhs.x * rhs,lhs.y * rhs };
}

inline DirectX::XMFLOAT2 operator/(const DirectX::XMFLOAT2& lhs, const float rhs)
{
    return { lhs.x / rhs,lhs.y / rhs };
}

inline DirectX::XMFLOAT2 operator*(float lhs, const DirectX::XMFLOAT2& rhs)
{
    return { lhs * rhs.x,lhs * rhs.y};
}

inline DirectX::XMFLOAT2 operator/(float lhs, const DirectX::XMFLOAT2& rhs)
{
    return { lhs / rhs.x,lhs / rhs.y };
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

// ��Ԃ̃x�N�g���̒������v�Z
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

inline float Length(const DirectX::XMFLOAT2& f1, const DirectX::XMFLOAT2& f2)
{
    float ret{};
    DirectX::XMStoreFloat(
        &ret,
        DirectX::XMVector3Length(
            DirectX::XMVectorSubtract(
                DirectX::XMLoadFloat2(&f1), DirectX::XMLoadFloat2(&f2)
            )
        )
    );
    return ret;
}


// ��̃x�N�g���̓��ς��v�Z���Ċp�xtheta���傫��������true
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

//���K��
inline DirectX::XMFLOAT3 Normalize(const DirectX::XMFLOAT3& f)
{
    DirectX::XMFLOAT3 ret{};
    DirectX::XMStoreFloat3(&ret, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&f)));
    return ret;
}

//���K��
inline DirectX::XMFLOAT2 Normalize(const DirectX::XMFLOAT2& f)
{
    DirectX::XMFLOAT2 ret{};
    DirectX::XMStoreFloat2(&ret, DirectX::XMVector2Normalize(DirectX::XMLoadFloat2(&f)));
    return ret;
}

// ����
inline float Dot(DirectX::XMFLOAT3 v1, DirectX::XMFLOAT3 v2)
{
    return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
}

// ����
inline float Dot(DirectX::XMFLOAT2 v1, DirectX::XMFLOAT2 v2)
{
    return (v1.x * v2.x + v1.y * v2.y );
}

// �O��
inline DirectX::XMFLOAT3 Cross(DirectX::XMFLOAT3 v1, DirectX::XMFLOAT3 v2)
{
    return { v1.y * v2.z - v1.z * v2.y ,v1.z * v2.x - v1.x * v2.z,v1.x * v2.y - v1.y * v2.x };
}

// ����
inline float Length(DirectX::XMFLOAT3 v1)
{
    return sqrtf(v1.x * v1.x + v1.y * v1.y + v1.z * v1.z);
}

// ����
inline float Length(DirectX::XMFLOAT2 v1)
{
    return sqrtf(v1.x * v1.x + v1.y * v1.y);
}

// float�^�̓���
inline bool Equal(const float x, const float y, const float epsilon = FLT_EPSILON)
{
    return (fabsf(x - y) < epsilon);
}


// ���W�����_���S�ŉ�]
inline DirectX::XMFLOAT2 RotatePoint(const DirectX::XMFLOAT2& point, const float angle)
{
    // ���W�����v���ɉ�]����
    float s = sin(angle);
    float c = cos(angle);
    DirectX::XMFLOAT2 rotatedPoint;
    rotatedPoint.x = point.x * c + point.y * s;
    rotatedPoint.y = -point.x * s + point.y * c; // y ���̕��������]
    return rotatedPoint;
}

// ��]��`�̒��S���W�A���Ɗp�x����Rect�����߂�
inline Rect CalcRotateRect(const DirectX::XMFLOAT2& center, const DirectX::XMFLOAT2& width, const float degree)
{
    // ��`�̕��ƍ���
    float halfWidth = width.x * 0.5f;
    float halfHeight = width.y * 0.5f;

    // ��`�̍���̍��W���v�Z
    DirectX::XMFLOAT2 rectTopLeft(-halfWidth, halfHeight);

    // ��]�s����쐬���ċ�`�̍��W����]
    DirectX::XMFLOAT2 rotatedTopLeft = RotatePoint(rectTopLeft, DirectX::XMConvertToRadians(degree));
    DirectX::XMFLOAT2 rotatedBottomRight = RotatePoint({ halfWidth, -halfHeight }, DirectX::XMConvertToRadians(degree));
    DirectX::XMFLOAT2 rotatedTopRight = RotatePoint({ halfWidth, halfHeight }, DirectX::XMConvertToRadians(degree));
    DirectX::XMFLOAT2 rotatedBottomLeft = RotatePoint({ -halfWidth, -halfHeight }, DirectX::XMConvertToRadians(degree));

    // ���S���W�����Z���ĉ�]��̍��W�����߂�
    Rect rect;
    rect.left_up = {
    center.x + rotatedTopLeft.x,
    center.y + rotatedTopLeft.y
    };
    rect.right_down = {
        center.x + rotatedBottomRight.x,
        center.y + rotatedBottomRight.y
    };
    rect.right_up = {
        center.x + rotatedTopRight.x,
        center.y + rotatedTopRight.y
    };
    rect.left_down = {
        center.x + rotatedBottomLeft.x,
        center.y + rotatedBottomLeft.y
    };
    rect.width = {
        width.x,
        width.y
    };
    rect.center = {
        center.x,
        center.y
    };

    return rect;
}
