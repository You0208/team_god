#pragma once

#include<DirectXMath.h>
#include "../Component/GameObject.h"

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
    // �l�p�Ɠ_�̓����蔻��
    static bool IntersectSquareVsPoint(
        const DirectX::XMFLOAT2& lu,
        const DirectX::XMFLOAT2& rd,
        const DirectX::XMFLOAT2& point);

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