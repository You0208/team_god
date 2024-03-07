#include "../Graphics/Graphics.h"
#include "Effect.h"
#include "EffekseerManager.h"

// コンストラク
Effect::Effect(const char* filename)
{
    // エフェクトを読み込む前にロックする
    // マルチスレッドでEffectを生成するとDeviceContextを同時アクセスして
    // フリーズする可能性があるので排他制御する
    std::lock_guard<std::mutex> lock(Lemur::Graphics::Graphics::Instance().GetMutex());

    // Effekseerのリソースを読み込む
    //effekseerはUTF-16以外は対応していないため文字コードの変換が必要
    char16_t utf16Filename[256];
    Effekseer::ConvertUtf8ToUtf16(utf16Filename, 256, filename);

    // Effekseer::Managerを取得
    Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

    //Effekseerエフェクトを読み込み
    effekseerEffect = Effekseer::Effect::Create(effekseerManager, (EFK_CHAR*)utf16Filename);
}

// 再生
Effekseer::Handle Effect::Play(const DirectX::XMFLOAT3& position, float scale)
{
    Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

    Effekseer::Handle handle = effekseerManager->Play(effekseerEffect, position.x, position.y, position.z);
    effekseerManager->SetScale(handle, scale, scale, scale);
    return handle;
}

void Effect::Stop(Effekseer::Handle handle)
{
    Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

    effekseerManager->StopEffect(handle);
}

// 座標設定
void Effect::SetPosition(Effekseer::Handle handle, const DirectX::XMFLOAT3& position)
{
    Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

    effekseerManager->SetLocation(handle, position.x, position.y, position.z);
}

// スケール設定
void Effect::SetScale(Effekseer::Handle handle, const DirectX::XMFLOAT3& scale)
{
    Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

    effekseerManager->SetScale(handle, scale.x, scale.y, scale.z);
}

// ローテーション設定
void Effect::SetRotation(Effekseer::Handle handle, const DirectX::XMFLOAT3& rotation)
{
    Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

    effekseerManager->SetRotation(handle, rotation.x, rotation.y, rotation.z);
}
