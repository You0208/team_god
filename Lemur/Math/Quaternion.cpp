//#include "Quaternion.h"
//
//Quaternion::Quaternion() :DirectX::XMFLOAT4(0, 0, 0, 1)
//{
//}
//
//Quaternion::~Quaternion()
//{
//}
//
//DirectX::XMFLOAT3 Quaternion::GetForward() const
//{
//    constexpr DirectX::XMFLOAT3 FORWARD = { 0,0,1 };
//
//    DirectX::XMFLOAT3 ret;
//    DirectX::XMStoreFloat3(&ret,
//        DirectX::XMVector3Rotate( //ベクトルをクォータニオンで回転させる。
//            DirectX::XMLoadFloat3(&FORWARD),// 回転させたい前ベクトル。
//            DirectX::XMLoadFloat4(this) // 回転値。
//        )
//    );
//    return ret;
//}
//
//DirectX::XMFLOAT3 Quaternion::GetRight() const
//{
//    constexpr DirectX::XMFLOAT3 RIGHT = { 1,0,0 };
//
//    DirectX::XMFLOAT3 ret;
//    DirectX::XMStoreFloat3(&ret,
//        DirectX::XMVector3Rotate(
//            DirectX::XMLoadFloat3(&RIGHT),
//            DirectX::XMLoadFloat4(this)
//        )
//    );
//    return ret;
//}
//
//DirectX::XMFLOAT3 Quaternion::GetUp() const
//{
//    constexpr DirectX::XMFLOAT3 UP = { 0,1,0 };
//
//    DirectX::XMFLOAT3 ret;
//    DirectX::XMStoreFloat3(&ret,
//        DirectX::XMVector3Rotate(
//            DirectX::XMLoadFloat3(&UP),
//            DirectX::XMLoadFloat4(this)
//        )
//    );
//    return ret;
//}
//
//DirectX::XMFLOAT3 Quaternion::RotateVector3(const DirectX::XMFLOAT3& v)const
//{
//    DirectX::XMFLOAT3 ret;
//    DirectX::XMStoreFloat3(&ret,
//        DirectX::XMVector3Rotate(
//            DirectX::XMLoadFloat3(&v),
//            DirectX::XMLoadFloat4(this)
//        )
//    );
//    return ret;
//}
//
//Quaternion Quaternion::operator*(const Quaternion& q)const
//{
//    Quaternion ret;
//    DirectX::XMStoreFloat4(
//        &ret,
//        DirectX::XMQuaternionMultiply(
//            DirectX::XMLoadFloat4(this),
//            DirectX::XMLoadFloat4(&q)
//        )
//    );
//    return ret;
//}
//
//
//Quaternion Quaternion::Slerp(const Quaternion& q1, const Quaternion& q2, float t)
//{
//    auto q1_ = DirectX::XMLoadFloat4(&q1);
//    auto q2_ = DirectX::XMLoadFloat4(&q2);
//    Quaternion ret;
//    DirectX::XMStoreFloat4(&ret, DirectX::XMQuaternionSlerp(q1_, q2_, t));
//    return ret;
//}
//
//Quaternion Quaternion::AngleAxis(const DirectX::XMFLOAT3& axis, float angle)
//{
//    Quaternion ret;
//    DirectX::XMStoreFloat4(&ret,
//        DirectX::XMQuaternionRotationAxis(DirectX::XMLoadFloat3(&axis), angle));
//    return ret;
//}
//
//Quaternion Quaternion::RollPitchYaw(float pitch, float yaw, float roll)
//{
//    Quaternion ret;
//    DirectX::XMStoreFloat4(&ret, DirectX::XMQuaternionRotationRollPitchYaw(pitch, yaw, roll));
//    return ret;
//}
//
//Quaternion Quaternion::LookTo(const DirectX::XMFLOAT3& vec, const DirectX::XMFLOAT3& up)
//{
//    DirectX::XMVECTOR eye = DirectX::XMVectorZero();
//    DirectX::XMVECTOR focus = DirectX::XMLoadFloat3(&vec);
//    DirectX::XMVECTOR upVec = DirectX::XMLoadFloat3(&up);
//
//    // 回転行列を作ってる
//    DirectX::XMMATRIX rotateMatrix =
//        DirectX::XMMatrixLookToLH(eye, focus, upVec);
//
//    rotateMatrix = DirectX::XMMatrixInverse(nullptr, rotateMatrix);
//
//    // 回転行列をクォータニオンにしてる。
//    DirectX::XMVECTOR rotateQuaternion =
//        DirectX::XMQuaternionRotationMatrix(rotateMatrix);
//
//    Quaternion ret;
//    DirectX::XMStoreFloat4(&ret, rotateQuaternion);
//    return ret;
//}
//
//float Quaternion::Angle(const Quaternion& q1, const Quaternion& q2)
//{
//    DirectX::XMFLOAT3 f1 = q1.GetForward();
//    DirectX::XMFLOAT3 f2 = q2.GetForward();
//    return DirectX::XMVectorGetX(
//        DirectX::XMVector3AngleBetweenNormals(
//            DirectX::XMLoadFloat3(&f1),
//            DirectX::XMLoadFloat3(&f2)
//        ));
//}
