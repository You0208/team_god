#include "Collision.h"


void CollisionPhysicsComponent::Initialize(GameObject* gameobj)
{

}

void CollisionPhysicsComponent::Update(GameObject* gameobj, float elapsedTime)
{

}


bool Collision::IntersectSquareVsPoint(const DirectX::XMFLOAT2& lu, const DirectX::XMFLOAT2& rd, const DirectX::XMFLOAT2& point)
{
    //l1������Al2���E��
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
    // A��B�̒P�ʃx�N�g�����Z�o
    float vx = positionB.x - positionA.x;//�x�N�g�������߂�
    float vz = positionB.y - positionA.y;//�x�N�g�������߂�
    float distXZ = sqrtf(vx * vx + vz * vz);//���������߂�
    float radius = radiusA + radiusB;//���a�̍��v�����߂�
    if (distXZ > radius)//��ׂ�
    {
        return false;
    }
    return true;
}

bool Collision::IntersectTriangleVsPoint(const Triangle T, const DirectX::XMFLOAT2& P)
{
    using namespace DirectX;
    //����͊O�Ƃ݂Ȃ��܂��B
    //ABC���O�p�`���ǂ����̃`�F�b�N�͏ȗ�...

    XMFLOAT2 AB = T.B - T.A;
    XMFLOAT2 BP = P - T.B;

    XMFLOAT2 BC = T.C - T.B;
    XMFLOAT2 CP = P - T.C;

    XMFLOAT2 CA = T.A - T.C;
    XMFLOAT2 AP = P - T.A;

    //�O��    Z���������v�Z����΂悢�ł�
    double c1 = AB.x * BP.y - AB.y * BP.x;
    double c2 = BC.x * CP.y - BC.y * CP.x;
    double c3 = CA.x * AP.y - CA.y * AP.x;

    if ((c1 > 0 && c2 > 0 && c3 > 0) || (c1 < 0 && c2 < 0 && c3 < 0)) {
        //�O�p�`�̓����ɓ_������
        return true;
    }

    //�O�p�`�̊O���ɓ_������
    return false;
}

bool Collision::IntersectTriangleVsCircle(const Triangle T, const DirectX::XMFLOAT2& P,const float radius)
{
    if (IntersectTriangleVsPoint(T, P))return true;
    else
    {
        // �e�����ƒ��S�_�̍ŒZ���������߁A���̒��������a�ȉ��Ȃ�ڐG

    }
    return false;
}



//�@���Ƌ���������
bool Collision::IntersectSphereVsSphereOut(
    const DirectX::XMFLOAT3& positionA,
    float radiusA,
    const DirectX::XMFLOAT3& positionB,
    float radiusB,
    DirectX::XMFLOAT3& outPositionB
)
{
    //// A��B�̒P�ʃx�N�g�����Z�o
    //DirectX::XMVECTOR PositionA = { positionA.x,positionA.y,positionA.z };
    //DirectX::XMVECTOR PositionB = { positionA.x,positionA.y,positionA.z };
    //DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(PositionB, PositionA);
    //DirectX::XMVECTOR LengthSq = DirectX::XMVector3Length(Vec);
    //float lengthSq;
    //DirectX::XMStoreFloat(&lengthSq, LengthSq);
    //
    //// ��������
    //float range = radiusA + radiusB;
    //if (range < lengthSq)
    //{
    //    return false;
    //}
    //
    //// A��B�������o������
    //DirectX::XMVECTOR OutVec = DirectX::XMVector3Normalize(Vec);
    //DirectX::XMVECTOR OutVecLeng = DirectX::XMVectorScale(OutVec,(range - lengthSq));
    //DirectX::XMVECTOR newPositionB = DirectX::XMVectorAdd(PositionB, OutVec);
    //DirectX::XMVECTOR NewPositionB = DirectX::XMVectorMultiplyAdd(OutVecLeng, OutVec, PositionB);
    //
    //// ���ʂ��o��
    //DirectX::XMStoreFloat3(&outPositionB, newPositionB);
    //
    //return true;

    // A��B�̒P�ʃx�N�g�����Z�o
    DirectX::XMVECTOR PositionA = DirectX::XMLoadFloat3(&positionA);
    DirectX::XMVECTOR PositionB = DirectX::XMLoadFloat3(&positionB);
    DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(PositionB, PositionA);
    DirectX::XMVECTOR LengthSq = DirectX::XMVector3LengthSq(Vec);//���[�g����ĂȂ��iC�̓��̏�ԁj
    float lengthSq;
    DirectX::XMStoreFloat(&lengthSq, LengthSq);

    // ��������
    float range = radiusA + radiusB;
    if (lengthSq > range * range)
    {
        return false;
    }

    // A��B�������o��
    Vec = DirectX::XMVector3Normalize(Vec);// �P�ʃx�N�g��
    Vec = DirectX::XMVectorScale(Vec, range);// �����𔼌a�̑������킹�����ɂ�
    PositionB = DirectX::XMVectorAdd(PositionA, Vec);//�`�̍��W�Ƀx�N�g���𑫂�
    DirectX::XMStoreFloat3(&outPositionB, PositionB);//�|�W�V�����a�����߂�

    return true;
}

//�@���Ƌ���������
bool Collision::IntersectSphereVsSphere(
    const DirectX::XMFLOAT3& positionA,
    float radiusA,
    const DirectX::XMFLOAT3& positionB,
    float radiusB
)
{
    // A��B�̒P�ʃx�N�g�����Z�o
    DirectX::XMVECTOR PositionA = DirectX::XMLoadFloat3(&positionA);
    DirectX::XMVECTOR PositionB = DirectX::XMLoadFloat3(&positionB);
    DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(PositionB, PositionA);
    DirectX::XMVECTOR LengthSq = DirectX::XMVector3LengthSq(Vec);//���[�g����ĂȂ��iC�̓��̏�ԁj
    float lengthSq;
    DirectX::XMStoreFloat(&lengthSq, LengthSq);

    // ��������
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
    // A�̑�����B�̓�����Ȃ瓖�����Ă��Ȃ�
    if (positionA.y > positionB.y + heightB)
    {
        return false;
    }
    // A�̓���B�̑�����艺�Ȃ瓖�����Ă��Ȃ�
    if (positionA.y + heightA < positionB.y)
    {
        return false;
    }

    // A��B�̒P�ʃx�N�g�����Z�o
    float vx = positionB.x - positionA.x;//�x�N�g�������߂�
    float vz = positionB.z - positionA.z;//�x�N�g�������߂�
    float distXZ = sqrtf(vx * vx + vz * vz);//���������߂�
    float radius = radiusA + radiusB;//���a�̍��v�����߂�
    if (distXZ > radius)//��ׂ�
    {
        return false;
    }

    vx = vx / distXZ;//�P�ʃx�N�g����
    vz = vz / distXZ;//�P�ʃx�N�g����
    outPositionB.x = positionA.x + (vx * radius);// A����P�ʃx�N�g�������ɔ��a�̍��v�������ړ�����
    outPositionB.y = positionB.y;
    outPositionB.z = positionA.z + (vz * radius);// A����P�ʃx�N�g�������ɔ��a�̍��v�������ړ�����

    return true;
}

bool Collision::IntersectCylinderVsCylinder(const DirectX::XMFLOAT3& positionA, float radiusA, float heightA, const DirectX::XMFLOAT3& positionB, float radiusB, float heightB)
{
    // A�̑�����B�̓�����Ȃ瓖�����Ă��Ȃ�
    if (positionA.y > positionB.y + heightB)
    {
        return false;
    }
    // A�̓���B�̑�����艺�Ȃ瓖�����Ă��Ȃ�
    if (positionA.y + heightA < positionB.y)
    {
        return false;
    }

    // A��B�̒P�ʃx�N�g�����Z�o
    float vx = positionB.x - positionA.x;//�x�N�g�������߂�
    float vz = positionB.z - positionA.z;//�x�N�g�������߂�
    float distXZ = sqrtf(vx * vx + vz * vz);//���������߂�
    float radius = radiusA + radiusB;//���a�̍��v�����߂�
    if (distXZ > radius)//��ׂ�
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
    // y�������蔻��
    if (spherePosition.y - sphereRadius > cylinderPosition.y + cylinderHeight)return false;
    if (spherePosition.y + sphereRadius < cylinderPosition.y)return false;

    // xz���ʓ����蔻��
    float vx = cylinderPosition.x - spherePosition.x;//�x�N�g�������߂�
    float vz = cylinderPosition.z - spherePosition.z;//�x�N�g�������߂�
    float radius = cylinderRadius + sphereRadius;//���a�̍��v�����߂�
    float distXZ = sqrtf(vx * vx + vz * vz);//���������߂�
    if (distXZ > radius)//��ׂ�
    {
        return false;
    }

    // �����~���������o��
    vx = vx / distXZ;//�P�ʃx�N�g����
    vz = vz / distXZ;//�P�ʃx�N�g����
    outCylinderPosition.x = spherePosition.x + (vx * radius);// A����P�ʃx�N�g�������ɔ��a�̍��v�������ړ�����
    outCylinderPosition.y = cylinderPosition.y;
    outCylinderPosition.z = spherePosition.z + (vz * radius);// A����P�ʃx�N�g�������ɔ��a�̍��v�������ړ�����

    return true;
}

bool Collision::IntersectSphereVsCylinder(const DirectX::XMFLOAT3& spherePosition, float sphereRadius,
    const DirectX::XMFLOAT3& cylinderPosition, float cylinderRadius, float cylinderHeight)
{
    // y�������蔻��
    if (spherePosition.y - sphereRadius > cylinderPosition.y + cylinderHeight)return false;
    if (spherePosition.y + sphereRadius < cylinderPosition.y)return false;

    // xz���ʓ����蔻��
    float vx = cylinderPosition.x - spherePosition.x;//�x�N�g�������߂�
    float vz = cylinderPosition.z - spherePosition.z;//�x�N�g�������߂�
    float radius = cylinderRadius + sphereRadius;//���a�̍��v�����߂�
    float distXZ = sqrtf(vx * vx + vz * vz);//���������߂�
    if (distXZ > radius)//��ׂ�
    {
        return false;
    }

    return true;
}
