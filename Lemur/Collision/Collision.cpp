#include "Collision.h"


void CollisionPhysicsComponent::Initialize(GameObject* gameobj)
{

}

void CollisionPhysicsComponent::Update(GameObject* gameobj, float elapsedTime)
{

}


bool Collision::IntersectSquareVsPoint(const DirectX::XMFLOAT2& lu, const DirectX::XMFLOAT2& rd, const DirectX::XMFLOAT2& point)
{
    //l1＝左上、l2＝右下
    if ((point.x > lu.x) && (point.x < rd.x) && (point.y < lu.y) && (point.y > rd.y)) return true;
    return false;
}

bool Collision::IntersectRectVsCircle(const Rect rect, const DirectX::XMFLOAT2& position, const float radius)
{
    if ((position.x > rect.left_up.x) && (position.x < rect.right_down.x) && (position.y < rect.left_up.y + radius) && (position.y > rect.right_down.y - radius)) return true;
    if ((position.x > rect.left_up.x - radius) && (position.x < rect.right_down.x + radius) && (position.y < rect.left_up.y) && (position.y > rect.right_down.y)) return true;
    return false;
}

bool Collision::IntersectCircleVsCircle(const DirectX::XMFLOAT2& positionA, const float radiusA, const DirectX::XMFLOAT2& positionB, const float radiusB)
{
    // A→Bの単位ベクトルを算出
    float vx = positionB.x - positionA.x;//ベクトルを求める
    float vz = positionB.y - positionA.y;//ベクトルを求める
    float distXZ = sqrtf(vx * vx + vz * vz);//距離を求める
    float radius = radiusA + radiusB;//半径の合計を求める
    if (distXZ > radius)//比べる
    {
        return false;
    }
    return true;
}

bool Collision::IntersectTriangleVsPoint(const Triangle T, const DirectX::XMFLOAT2& P)
{
    using namespace DirectX;
    //線上は外とみなします。
    //ABCが三角形かどうかのチェックは省略...

    XMFLOAT2 AB = T.B - T.A;
    XMFLOAT2 BP = P - T.B;

    XMFLOAT2 BC = T.C - T.B;
    XMFLOAT2 CP = P - T.C;

    XMFLOAT2 CA = T.A - T.C;
    XMFLOAT2 AP = P - T.A;

    //外積    Z成分だけ計算すればよいです
    double c1 = AB.x * BP.y - AB.y * BP.x;
    double c2 = BC.x * CP.y - BC.y * CP.x;
    double c3 = CA.x * AP.y - CA.y * AP.x;

    if ((c1 > 0 && c2 > 0 && c3 > 0) || (c1 < 0 && c2 < 0 && c3 < 0)) {
        //三角形の内側に点がある
        return true;
    }

    //三角形の外側に点がある
    return false;
}

bool Collision::IntersectTriangleVsCircle(const Triangle T, const DirectX::XMFLOAT2& P,const float radius)
{
    if (IntersectTriangleVsPoint(T, P))return true;
    else
    {
        // 各線分と中心点の最短距離を求め、その長さが半径以下なら接触

    }
    return false;
}



//　球と球交差判定
bool Collision::IntersectSphereVsSphereOut(
    const DirectX::XMFLOAT3& positionA,
    float radiusA,
    const DirectX::XMFLOAT3& positionB,
    float radiusB,
    DirectX::XMFLOAT3& outPositionB
)
{
    //// A→Bの単位ベクトルを算出
    //DirectX::XMVECTOR PositionA = { positionA.x,positionA.y,positionA.z };
    //DirectX::XMVECTOR PositionB = { positionA.x,positionA.y,positionA.z };
    //DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(PositionB, PositionA);
    //DirectX::XMVECTOR LengthSq = DirectX::XMVector3Length(Vec);
    //float lengthSq;
    //DirectX::XMStoreFloat(&lengthSq, LengthSq);
    //
    //// 距離判定
    //float range = radiusA + radiusB;
    //if (range < lengthSq)
    //{
    //    return false;
    //}
    //
    //// AがBを押し出す処理
    //DirectX::XMVECTOR OutVec = DirectX::XMVector3Normalize(Vec);
    //DirectX::XMVECTOR OutVecLeng = DirectX::XMVectorScale(OutVec,(range - lengthSq));
    //DirectX::XMVECTOR newPositionB = DirectX::XMVectorAdd(PositionB, OutVec);
    //DirectX::XMVECTOR NewPositionB = DirectX::XMVectorMultiplyAdd(OutVecLeng, OutVec, PositionB);
    //
    //// 結果を出力
    //DirectX::XMStoreFloat3(&outPositionB, newPositionB);
    //
    //return true;

    // A→Bの単位ベクトルを算出
    DirectX::XMVECTOR PositionA = DirectX::XMLoadFloat3(&positionA);
    DirectX::XMVECTOR PositionB = DirectX::XMLoadFloat3(&positionB);
    DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(PositionB, PositionA);
    DirectX::XMVECTOR LengthSq = DirectX::XMVector3LengthSq(Vec);//ルート取ってない（Cの二乗の状態）
    float lengthSq;
    DirectX::XMStoreFloat(&lengthSq, LengthSq);

    // 距離判定
    float range = radiusA + radiusB;
    if (lengthSq > range * range)
    {
        return false;
    }

    // AがBを押し出す
    Vec = DirectX::XMVector3Normalize(Vec);// 単位ベクトル
    Vec = DirectX::XMVectorScale(Vec, range);// 長さを半径の足し合わせた分にｎ
    PositionB = DirectX::XMVectorAdd(PositionA, Vec);//Ａの座標にベクトルを足す
    DirectX::XMStoreFloat3(&outPositionB, PositionB);//ポジションＢを求める

    return true;
}

//　球と球交差判定
bool Collision::IntersectSphereVsSphere(
    const DirectX::XMFLOAT3& positionA,
    float radiusA,
    const DirectX::XMFLOAT3& positionB,
    float radiusB
)
{
    // A→Bの単位ベクトルを算出
    DirectX::XMVECTOR PositionA = DirectX::XMLoadFloat3(&positionA);
    DirectX::XMVECTOR PositionB = DirectX::XMLoadFloat3(&positionB);
    DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(PositionB, PositionA);
    DirectX::XMVECTOR LengthSq = DirectX::XMVector3LengthSq(Vec);//ルート取ってない（Cの二乗の状態）
    float lengthSq;
    DirectX::XMStoreFloat(&lengthSq, LengthSq);

    // 距離判定
    float range = radiusA + radiusB;
    if (lengthSq > range * range)
    {
        return false;
    }

    return true;
}

bool Collision::IntersectCylinderVsCylinder(
    const DirectX::XMFLOAT3& positionA,
    float radiusA,
    float heightA,
    const DirectX::XMFLOAT3& positionB,
    float radiusB,
    float heightB,
    DirectX::XMFLOAT3& outPositionB)
{
    // Aの足元がBの頭より上なら当たっていない
    if (positionA.y > positionB.y + heightB)
    {
        return false;
    }
    // Aの頭がBの足元より下なら当たっていない
    if (positionA.y + heightA < positionB.y)
    {
        return false;
    }

    // A→Bの単位ベクトルを算出
    float vx = positionB.x - positionA.x;//ベクトルを求める
    float vz = positionB.z - positionA.z;//ベクトルを求める
    float distXZ = sqrtf(vx * vx + vz * vz);//距離を求める
    float radius = radiusA + radiusB;//半径の合計を求める
    if (distXZ > radius)//比べる
    {
        return false;
    }

    vx = vx / distXZ;//単位ベクトル化
    vz = vz / distXZ;//単位ベクトル化
    outPositionB.x = positionA.x + (vx * radius);// Aから単位ベクトル方向に半径の合計分だけ移動する
    outPositionB.y = positionB.y;
    outPositionB.z = positionA.z + (vz * radius);// Aから単位ベクトル方向に半径の合計分だけ移動する

    return true;
}

bool Collision::IntersectCylinderVsCylinder(const DirectX::XMFLOAT3& positionA, float radiusA, float heightA, const DirectX::XMFLOAT3& positionB, float radiusB, float heightB)
{
    // Aの足元がBの頭より上なら当たっていない
    if (positionA.y > positionB.y + heightB)
    {
        return false;
    }
    // Aの頭がBの足元より下なら当たっていない
    if (positionA.y + heightA < positionB.y)
    {
        return false;
    }

    // A→Bの単位ベクトルを算出
    float vx = positionB.x - positionA.x;//ベクトルを求める
    float vz = positionB.z - positionA.z;//ベクトルを求める
    float distXZ = sqrtf(vx * vx + vz * vz);//距離を求める
    float radius = radiusA + radiusB;//半径の合計を求める
    if (distXZ > radius)//比べる
    {
        return false;
    }
    return true;
}

bool Collision::IntersectSphereVsCylinderOut(
    const DirectX::XMFLOAT3& spherePosition,
    float sphereRadius,
    const DirectX::XMFLOAT3& cylinderPosition,
    float cylinderRadius, float cylinderHeight,
    DirectX::XMFLOAT3& outCylinderPosition)
{
    // y軸当たり判定
    if (spherePosition.y - sphereRadius > cylinderPosition.y + cylinderHeight)return false;
    if (spherePosition.y + sphereRadius < cylinderPosition.y)return false;

    // xz平面当たり判定
    float vx = cylinderPosition.x - spherePosition.x;//ベクトルを求める
    float vz = cylinderPosition.z - spherePosition.z;//ベクトルを求める
    float radius = cylinderRadius + sphereRadius;//半径の合計を求める
    float distXZ = sqrtf(vx * vx + vz * vz);//距離を求める
    if (distXZ > radius)//比べる
    {
        return false;
    }

    // 球が円柱を押し出す
    vx = vx / distXZ;//単位ベクトル化
    vz = vz / distXZ;//単位ベクトル化
    outCylinderPosition.x = spherePosition.x + (vx * radius);// Aから単位ベクトル方向に半径の合計分だけ移動する
    outCylinderPosition.y = cylinderPosition.y;
    outCylinderPosition.z = spherePosition.z + (vz * radius);// Aから単位ベクトル方向に半径の合計分だけ移動する

    return true;
}

bool Collision::IntersectSphereVsCylinder(const DirectX::XMFLOAT3& spherePosition, float sphereRadius,
    const DirectX::XMFLOAT3& cylinderPosition, float cylinderRadius, float cylinderHeight)
{
    // y軸当たり判定
    if (spherePosition.y - sphereRadius > cylinderPosition.y + cylinderHeight)return false;
    if (spherePosition.y + sphereRadius < cylinderPosition.y)return false;

    // xz平面当たり判定
    float vx = cylinderPosition.x - spherePosition.x;//ベクトルを求める
    float vz = cylinderPosition.z - spherePosition.z;//ベクトルを求める
    float radius = cylinderRadius + sphereRadius;//半径の合計を求める
    float distXZ = sqrtf(vx * vx + vz * vz);//距離を求める
    if (distXZ > radius)//比べる
    {
        return false;
    }

    return true;
}
