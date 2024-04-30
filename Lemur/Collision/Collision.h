#pragma once

#include<DirectXMath.h>
#include "../Component/GameObject.h"
#include "../Math/MathHelper.h"

struct HitResult
{
    DirectX::XMFLOAT3 position = { 0,0,0 }; // レイとポリゴンの交点
    DirectX::XMFLOAT3 normal = { 0,0,0 };	// 衝突したポリゴンの法線ベクトル
    float distance = 0.0f;					// レイの始点から交点までの距離
    int materialIndex = -1;					// 衝突したポリゴンのマテリアル番号
    DirectX::XMFLOAT3 rotation = { 0,0,0 }; // 回転量
};

class CollisionPhysicsComponent :public PhysicsComponent
{
    void Initialize(GameObject* gameobj) override;
    void Update(GameObject* gameobj, float elapsedTime) override;

};

// コリジョン
class Collision
{
public:
    // 2D
    // 四角と点の当たり判定
    static bool IntersectSquareVsPoint(
        const DirectX::XMFLOAT2& lu,
        const DirectX::XMFLOAT2& rd,
        const DirectX::XMFLOAT2& point);

    // 四角と円の当たり判定
    static bool IntersectRectVsCircle(
        const Rect rect,
        const DirectX::XMFLOAT2& position,
        const float radius
    );

    // 円と円の当たり判定
    static bool IntersectCircleVsCircle(
        const DirectX::XMFLOAT2& positionA,
        const float radiusA,
        const DirectX::XMFLOAT2& positionB,
        const float radiusB
    );

    // 三角形の内点判定
    static bool IntersectTriangleVsPoint(
        const Triangle T,
        const DirectX::XMFLOAT2& P
    );

    // 三角形と円の当たり判定
    static bool IntersectTriangleVsCircle(
        const Triangle T,
        const DirectX::XMFLOAT2& P,
        const float radius
    );

    // 円と線分の当たり判定
    static bool IntersectCircleVsLine(
        const DirectX::XMFLOAT2& L_S,
        const DirectX::XMFLOAT2& L_E,
        const DirectX::XMFLOAT2& P,
        const float radius
    );

    // 円と点の当たり判定
    static bool IntersectCircleVsPosition(
        const DirectX::XMFLOAT2& position,
        const DirectX::XMFLOAT2& O,
        const float radius
    );

    // 回転矩形と点の当たり判定
    static bool IntersectRotateRectVsPoint(
        const Rect& rect,
        const DirectX::XMFLOAT2& position,
        const float angle
    );

    // 回転矩形と円の当たり判定
    static bool IntersectRotateRectVsCircle(
        const Rect& rect,
        const DirectX::XMFLOAT2& position,
        const float radius,
        const float angle
    );

    // ドーナツ型と円の当たり判定
    static bool IntersectDonutVsCircle(
        const DirectX::XMFLOAT2& positionA,
        const float radius_out,
        const float radius_in,
        const DirectX::XMFLOAT2& positionB,
        const float radiusB
    );

    // 3D
    // 球と球の交差判定
    static bool IntersectSphereVsSphereOut(
        const DirectX::XMFLOAT3& positionA,
        float radiusA,
        const DirectX::XMFLOAT3& positionB,
        float radiusB,
        DirectX::XMFLOAT3& outPositionB
    );

    // 球と球の交差判定（押し出しなし）
    static bool IntersectSphereVsSphere(
        const DirectX::XMFLOAT3& positionA,
        float radiusA,
        const DirectX::XMFLOAT3& positionB,
        float radiusB
    );

    // 円柱と円柱の交差判定
    static bool IntersectCylinderVsCylinder(
        const DirectX::XMFLOAT3& positionA,
        float radiusA,
        float heightA,
        const DirectX::XMFLOAT3& positionB,
        float radiusB,
        float heightB,
        DirectX::XMFLOAT3& outPositionB
    );

    // 円柱と円柱の交差判定(outPositionなし)
    static bool IntersectCylinderVsCylinder(
        const DirectX::XMFLOAT3& positionA,
        float radiusA,
        float heightA,
        const DirectX::XMFLOAT3& positionB,
        float radiusB,
        float heightB
    );

    // 球と円柱の交差判定
    static bool IntersectSphereVsCylinderOut(
        const DirectX::XMFLOAT3& spherePosition,
        float sphereradius,
        const DirectX::XMFLOAT3& cylinderPosition,
        float cylinderradius,
        float cylinderheigh,
        DirectX::XMFLOAT3& outCylinderPosition
    );

    // 球と円柱の交差判定（押し出しなし）
    static bool IntersectSphereVsCylinder(
        const DirectX::XMFLOAT3& spherePosition,
        float sphereradius,
        const DirectX::XMFLOAT3& cylinderPosition,
        float cylinderradius,
        float cylinderheigh
    );

};