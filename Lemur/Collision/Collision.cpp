#include "Collision.h"
#include "Lemur/Graphics/DebugRenderer.h"


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

    XMFLOAT2 AB = T.B - T.A;
    XMFLOAT2 BP = P - T.B;

    XMFLOAT2 BC = T.C - T.B;
    XMFLOAT2 CP = P - T.C;

    XMFLOAT2 CA = T.A - T.C;
    XMFLOAT2 AP = P - T.A;

    //外積
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

bool Collision::IntersectTriangleVsCircle(const Triangle T, const DirectX::XMFLOAT2& P, const float radius)
{
    // 内点判定
    if (IntersectTriangleVsPoint(T, P))
    {
        return true;
    }
    else
    {
        // 三角の各辺と円の当たり判定
        bool judge1 = IntersectCircleVsLine(T.A, T.B, P, radius);
        bool judge2 = IntersectCircleVsLine(T.B, T.C, P, radius);
        bool judge3 = IntersectCircleVsLine(T.C, T.A, P, radius);
        if (judge1 || judge2 || judge3)return true;
    }
    return false;
}

bool Collision::IntersectCircleVsLine(const DirectX::XMFLOAT2& L_S, const DirectX::XMFLOAT2& L_E, const DirectX::XMFLOAT2& P, const float radius)
{
    using namespace DirectX;
    // ベクトルの作成
    XMFLOAT2 start_to_center = P - L_S;
    XMFLOAT2 end_to_center = P - L_E;
    XMFLOAT2 start_to_end = L_E - L_S;

    // 単位ベクトル化する
    XMFLOAT2 normal_start_to_end = Normalize(start_to_end);

    /*
        射影した線分の長さ
            始点と円の中心で外積を行う
            ※始点 => 終点のベクトルは単位化しておく
    */
    float distance_projection = start_to_center.x * normal_start_to_end.y - normal_start_to_end.x * start_to_center.y;

    // 線分と円の最短の長さが半径よりも小さい
    if (fabs(distance_projection) < radius)
    {
        // 始点 => 終点と始点 => 円の中心の内積を計算する
        float dot01 = start_to_center.x * start_to_end.x + start_to_center.y * start_to_end.y;
        // 始点 => 終点と終点 => 円の中心の内積を計算する
        float dot02 = end_to_center.x * start_to_end.x + end_to_center.y * start_to_end.y;

        // 二つの内積の掛け算結果が0以下なら当たり
        if (dot01 * dot02 <= 0.0f)
        {
            return true;
        }
        /*
    上の条件から漏れた場合、円は線分上にはないので、
    始点 => 円の中心の長さか、終点 => 円の中心の長さが
    円の半径よりも短かったら当たり
        */
        if (Length(start_to_center) < radius ||
            Length(end_to_center) < radius)
        {
            return true;
        }
    }
    else
    {
        return false;
    }

    return false;
}

bool Collision::IntersectCircleVsPosition(const DirectX::XMFLOAT2& position, const DirectX::XMFLOAT2& o, const float radius)
{
    float dx = position.x - o.x;
    float dy = position.y - o.y;
    float d = dx * dx + dy * dy;
    if (d < radius * radius)
    {
        return true;
    }

    return false;
}

bool Collision::IntersectRotateRectVsPoint(const Rect& rect, const DirectX::XMFLOAT2& position, const float angle)
{
    // 回転矩形の中心を原点としたときの各頂点座標
    float left = rect.center.x - (rect.width.x * 0.5f);
    float right = rect.center.x + (rect.width.x * 0.5f);
    float top = rect.center.y + (rect.width.y * 0.5f);
    float bottom = rect.center.y - (rect.width.y * 0.5f);

    //矩形の中心と点の距離を計算
    float l;
    l = sqrt(((position.x - rect.center.x) * (position.x - rect.center.x)) + ((position.y - rect.center.y) * (position.y - rect.center.y)));

    //矩形の中心を原点としてみた相対的な点Cの座標
    DirectX::XMFLOAT2 p2;
    p2.x = position.x - rect.center.x;
    p2.y = position.y - rect.center.y;

    DirectX::XMFLOAT2  p3;
    // 相対座標に対して矩形の回転を打ち消す逆行列を掛ける
    p3 = {
        cosf(DirectX::XMConvertToRadians(-angle)) * p2.x + sinf(DirectX::XMConvertToRadians(-angle)) * p2.y,
        -sinf(DirectX::XMConvertToRadians(-angle)) * p2.x + cosf(DirectX::XMConvertToRadians(-angle)) * p2.y
    };

    //普通の矩形と点の当たり判定
    if (left <= p3.x && p3.x <= right && bottom <= p3.y && p3.y <= top)
    {
        return true;
    }
    return false;
}

bool Collision::IntersectRotateRectVsCircle(const Rect& rect, const DirectX::XMFLOAT2& position, const float radius,const float angle)
{    
    //回転矩形と点の当たり判定
    if (IntersectRotateRectVsPoint(rect,position,angle))
    {
        return true;
    }
    else
    {
        // 左下
        bool judge1 = IntersectCircleVsLine(rect.left_up, rect.left_down, position, radius);
        bool judge2 = IntersectCircleVsLine(rect.left_down, rect.right_down, position, radius);
        bool judge3 = IntersectCircleVsLine(rect.right_down, rect.right_up, position, radius);
        bool judge4 = IntersectCircleVsLine(rect.right_up, rect.left_up, position, radius);
        if (judge1 || judge2 || judge3 || judge4)return true;
    }
    return false;
}

bool Collision::IntersectDonutVsCircle(const DirectX::XMFLOAT2& donut_pos, const float radius_out, const float radius_in, const DirectX::XMFLOAT2& position, const float radius)
{
    if (IntersectCircleVsCircle(donut_pos, radius_in, position, radius))
    {
        return false;
    }
    else if (IntersectCircleVsCircle(donut_pos, radius_out, position, radius))
    {
        return true;
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
