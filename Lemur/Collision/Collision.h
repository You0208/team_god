#pragma once

#include<DirectXMath.h>
#include "../Component/GameObject.h"
#include "../Math/MathHelper.h"

struct HitResult
{
    DirectX::XMFLOAT3 position = { 0,0,0 }; // ���C�ƃ|���S���̌�_
    DirectX::XMFLOAT3 normal = { 0,0,0 };	// �Փ˂����|���S���̖@���x�N�g��
    float distance = 0.0f;					// ���C�̎n�_�����_�܂ł̋���
    int materialIndex = -1;					// �Փ˂����|���S���̃}�e���A���ԍ�
    DirectX::XMFLOAT3 rotation = { 0,0,0 }; // ��]��
};

class CollisionPhysicsComponent :public PhysicsComponent
{
    void Initialize(GameObject* gameobj) override;
    void Update(GameObject* gameobj, float elapsedTime) override;

};

// �R���W����
class Collision
{
public:
    // 2D
    // �l�p�Ɠ_�̓����蔻��
    static bool IntersectSquareVsPoint(
        const DirectX::XMFLOAT2& lu,
        const DirectX::XMFLOAT2& rd,
        const DirectX::XMFLOAT2& point);

    // �l�p�Ɖ~�̓����蔻��
    static bool IntersectRectVsCircle(
        const Rect rect,
        const DirectX::XMFLOAT2& position,
        const float radius
    );

    // �~�Ɖ~�̓����蔻��
    static bool IntersectCircleVsCircle(
        const DirectX::XMFLOAT2& positionA,
        const float radiusA,
        const DirectX::XMFLOAT2& positionB,
        const float radiusB
    );

    // �O�p�`�̓��_����
    static bool IntersectTriangleVsPoint(
        const Triangle T,
        const DirectX::XMFLOAT2& P
    );

    // �O�p�`�Ɖ~�̓����蔻��
    static bool IntersectTriangleVsCircle(
        const Triangle T,
        const DirectX::XMFLOAT2& P,
        const float radius
    );

    // �~�Ɛ����̓����蔻��
    static bool IntersectCircleVsLine(
        const DirectX::XMFLOAT2& L_S,
        const DirectX::XMFLOAT2& L_E,
        const DirectX::XMFLOAT2& P,
        const float radius
    );

    // �~�Ɠ_�̓����蔻��
    static bool IntersectCircleVsPosition(
        const DirectX::XMFLOAT2& position,
        const DirectX::XMFLOAT2& O,
        const float radius
    );

    // ��]��`�Ɠ_�̓����蔻��
    static bool IntersectRotateRectVsPoint(
        const Rect& rect,
        const DirectX::XMFLOAT2& position,
        const float angle
    );

    // ��]��`�Ɖ~�̓����蔻��
    static bool IntersectRotateRectVsCircle(
        const Rect& rect,
        const DirectX::XMFLOAT2& position,
        const float radius,
        const float angle
    );

    // �h�[�i�c�^�Ɖ~�̓����蔻��
    static bool IntersectDonutVsCircle(
        const DirectX::XMFLOAT2& positionA,
        const float radius_out,
        const float radius_in,
        const DirectX::XMFLOAT2& positionB,
        const float radiusB
    );

    // 3D
    // ���Ƌ��̌�������
    static bool IntersectSphereVsSphereOut(
        const DirectX::XMFLOAT3& positionA,
        float radiusA,
        const DirectX::XMFLOAT3& positionB,
        float radiusB,
        DirectX::XMFLOAT3& outPositionB
    );

    // ���Ƌ��̌�������i�����o���Ȃ��j
    static bool IntersectSphereVsSphere(
        const DirectX::XMFLOAT3& positionA,
        float radiusA,
        const DirectX::XMFLOAT3& positionB,
        float radiusB
    );

    // �~���Ɖ~���̌�������
    static bool IntersectCylinderVsCylinder(
        const DirectX::XMFLOAT3& positionA,
        float radiusA,
        float heightA,
        const DirectX::XMFLOAT3& positionB,
        float radiusB,
        float heightB,
        DirectX::XMFLOAT3& outPositionB
    );

    // �~���Ɖ~���̌�������(outPosition�Ȃ�)
    static bool IntersectCylinderVsCylinder(
        const DirectX::XMFLOAT3& positionA,
        float radiusA,
        float heightA,
        const DirectX::XMFLOAT3& positionB,
        float radiusB,
        float heightB
    );

    // ���Ɖ~���̌�������
    static bool IntersectSphereVsCylinderOut(
        const DirectX::XMFLOAT3& spherePosition,
        float sphereradius,
        const DirectX::XMFLOAT3& cylinderPosition,
        float cylinderradius,
        float cylinderheigh,
        DirectX::XMFLOAT3& outCylinderPosition
    );

    // ���Ɖ~���̌�������i�����o���Ȃ��j
    static bool IntersectSphereVsCylinder(
        const DirectX::XMFLOAT3& spherePosition,
        float sphereradius,
        const DirectX::XMFLOAT3& cylinderPosition,
        float cylinderradius,
        float cylinderheigh
    );

};